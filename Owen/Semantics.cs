using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Owen
{
    internal static class Semantics
    {
        private static readonly PrimitiveType I8 = new PrimitiveType() { Tag = PrimitiveTypeTag.I8 };
        private static readonly PrimitiveType I16 = new PrimitiveType() { Tag = PrimitiveTypeTag.I16 };
        private static readonly PrimitiveType I32 = new PrimitiveType() { Tag = PrimitiveTypeTag.I32 };
        private static readonly PrimitiveType I64 = new PrimitiveType() { Tag = PrimitiveTypeTag.I64 };
        private static readonly PrimitiveType U8 = new PrimitiveType() { Tag = PrimitiveTypeTag.U8 };
        private static readonly PrimitiveType U16 = new PrimitiveType() { Tag = PrimitiveTypeTag.U16 };
        private static readonly PrimitiveType U32 = new PrimitiveType() { Tag = PrimitiveTypeTag.U32 };
        private static readonly PrimitiveType U64 = new PrimitiveType() { Tag = PrimitiveTypeTag.U64 };
        private static readonly PrimitiveType F32 = new PrimitiveType() { Tag = PrimitiveTypeTag.F32 };
        private static readonly PrimitiveType F64 = new PrimitiveType() { Tag = PrimitiveTypeTag.F64 };
        private static readonly PrimitiveType Bool = new PrimitiveType() { Tag = PrimitiveTypeTag.Bool };

        public static void Analyze(Program program)
        {
            program.Scope.Symbols.Add(new Symbol() { Name = I8.Tag.ToString(), Type = I8 });
            program.Scope.Symbols.Add(new Symbol() { Name = I16.Tag.ToString(), Type = I16 });
            program.Scope.Symbols.Add(new Symbol() { Name = I32.Tag.ToString(), Type = I32 });
            program.Scope.Symbols.Add(new Symbol() { Name = I64.Tag.ToString(), Type = I64 });
            program.Scope.Symbols.Add(new Symbol() { Name = U8.Tag.ToString(), Type = U8 });
            program.Scope.Symbols.Add(new Symbol() { Name = U16.Tag.ToString(), Type = U16 });
            program.Scope.Symbols.Add(new Symbol() { Name = U32.Tag.ToString(), Type = U32 });
            program.Scope.Symbols.Add(new Symbol() { Name = U64.Tag.ToString(), Type = U64 });
            program.Scope.Symbols.Add(new Symbol() { Name = F32.Tag.ToString(), Type = F32 });
            program.Scope.Symbols.Add(new Symbol() { Name = F64.Tag.ToString(), Type = F64 });
            program.Scope.Symbols.Add(new Symbol() { Name = Bool.Tag.ToString(), Type = Bool });

            ResolveNamespaces(program);
            foreach (var enumeration in program.Files.SelectMany(f => f.Enumerations))
                Analyze(enumeration, program.Scope);

            foreach (var file in program.Files)
            {
                foreach (var compound in file.Compounds)
                {
                    var fieldWithTheSameNameAsDeclaration = compound.Fields.FirstOrDefault(f => f.Name.Value == compound.Name.Value);
                    if (fieldWithTheSameNameAsDeclaration != null)
                        Report.Error($"{fieldWithTheSameNameAsDeclaration.Name.Start} Fields cannot have the same name as the compound.");

                    foreach (var field in compound.Fields)
                    {
                        var positionOfType = ((UnresolvedType)field.Type).Identifier.Start;

                        field.Type = Analyze(field.Type, file.Scope);
                        if (field.Type == null)
                            Report.Error($"{positionOfType} {field.Name.Value} is undefined.");
                    }

                    foreach (var a in compound.Fields)
                    {
                        foreach (var b in compound.Fields)
                        {
                            if (a != b && a.Name.Value == b.Name.Value)
                                Report.Error($"{b.Name.Start} Redeclares {b.Name.Value}.");
                        }
                    }
                }
            }

            foreach (var file in program.Files)
            {
                foreach (var compound in file.Compounds)
                {
                    void CheckIfFieldIsRecursive(Field parent, List<Field> trace)
                    {
                        if (parent.Type is CompoundDeclaration c)
                        {
                            trace.Add(parent);
                            foreach (var field in c.Fields)
                            {
                                if (trace[0] == field)
                                {
                                    if (trace.Count == 1)
                                        Report.Error($"{field.Name.Start} The structure is directly recursive.");
                                    else if (trace.Count > 1)
                                        Report.Error($"{((CompoundDeclaration)trace[1].Type).Name.Start} The structure is indirectly recursive:{Environment.NewLine}{string.Join(Environment.NewLine, trace.Select(a => $"{a.Name.Start} {a.Name.Value}"))}");
                                }
                                else if (field.Type is CompoundDeclaration)
                                    CheckIfFieldIsRecursive(field, trace);
                            }

                            trace.RemoveAt(trace.Count - 1);
                        }
                    }

                    foreach (var field in compound.Fields)
                        CheckIfFieldIsRecursive(field, new List<Field>());
                }
            }

            foreach (var file in program.Files)
            {
                foreach (var function in file.Functions)
                    AnalyzeSignature(file.Scope, function);
            }

            foreach (var function in program.Files.SelectMany(f => f.Functions))
                AnalyzeBody(function);

            foreach (var file in program.Files)
                Analyze(file.Propositions, program.Scope);

            var mainFunctions = program.Files.SelectMany(f => f.Functions)
                                             .Where(f => f.Name.Value == "main")
                                             .ToList();

            if (mainFunctions.Count == 0)
                Report.Error("No main function defined.");
            else if (mainFunctions.Count > 1)
                Report.Error($"Multiple main functons defined:\r\n{string.Join("\r\n", mainFunctions.Select(f => f.Name.Start))}");

            var main = mainFunctions[0];
            if (!Compare(main.Output, I32))
                Report.Error($"{main.Name.Start} main must output a single {I32}.");
        }

        private static void ResolveNamespaces(Program program)
        {
            foreach (var file in program.Files)
            {
                file.Scope = new Scope();
                file.Scope.Parent = program.Scope;

                var visibleFiles = program.Files.Where(f => f.Namespace.Value == file.Namespace.Value || file.Uses.Any(u => u.Value == f.Namespace.Value));
                foreach (var otherFile in visibleFiles)
                {
                    if (file.Path != otherFile.Path && !file.PathsToRenferencedFiles.Contains(otherFile.Path))
                        file.PathsToRenferencedFiles.Add(otherFile.Path);

                    foreach (var enumeration in otherFile.Enumerations.Where(e => file.Enumerations.Contains(e) || e.IsPublic))
                    {
                        file.Scope.Symbols.Add(new Symbol()
                        {
                            Name = enumeration.Name.Value,
                            Type = enumeration
                        });
                    }
                }

                foreach (var otherFile in visibleFiles)
                {
                    if (file.Path != otherFile.Path && !file.PathsToRenferencedFiles.Contains(otherFile.Path))
                        file.PathsToRenferencedFiles.Add(otherFile.Path);

                    foreach (var compound in otherFile.Compounds.Where(c => file.Compounds.Contains(c) || c.IsPublic))
                    {
                        file.Scope.Symbols.Add(new Symbol()
                        {
                            Name = compound.Name.Value,
                            Type = compound
                        });
                    }
                }

                foreach (var otherFile in visibleFiles)
                {
                    if (file.Path != otherFile.Path && !file.PathsToRenferencedFiles.Contains(otherFile.Path))
                        file.PathsToRenferencedFiles.Add(otherFile.Path);

                    foreach (var function in otherFile.Functions.Where(f => file.Functions.Contains(f) || f.IsPublic))
                    {
                        file.Scope.Symbols.Add(new Symbol()
                        {
                            Name = function.Name.Value,
                            Type = function
                        });
                    }
                }
            }
        }

        private static void AnalyzeSignature(Scope parent, FunctionDeclaration function)
        {
            bool IsGenericType(Type t)
            {
                if (t is UnresolvedType unresolved)
                    return !function.Generalized.Any(g => unresolved.Identifier.Value == g.Value);
                else if (t is Pointer pointer)
                    return IsGenericType(pointer.To);

                return true;
            }

            function.Body.Scope.Parent = parent;
            for (var i = 0; i < function.Input.Count; i++)
            {
                if (IsGenericType(function.Input[i].Type))
                    function.Input[i].Type = Analyze(function.Input[i].Type, parent);
            }

            if (function.Output is TupleType tuple)
            {
                for (var i = 0; i < tuple.Types.Count; i++)
                {
                    if (IsGenericType(tuple.Types[i]))
                        tuple.Types[i] = Analyze(tuple.Types[i], parent);
                }
            }
            else if (function.Output != null && IsGenericType(function.Output))
                function.Output = Analyze(function.Output, parent);
        }

        private static void AnalyzeBody(FunctionDeclaration function)
        {
            var index = default(ushort);
            foreach (var argument in function.Input)
            {
                if (NullableLookup(argument.Name, function.Body.Scope) is PrimitiveType primitive)
                    Report.Error($"{argument.Name.Start} Redeclares {argument.Name.Value}.");
                else
                {
                    function.Body.Scope.Symbols.Add(new InputSymbol()
                    {
                        Name = argument.Name.Value,
                        Type = argument.Type,
                        Index = index++
                    });
                }
            }

            if (function.Generalized.Count == 0)
                Analyze(function.Body, function.Output, function.Body.Scope);
        }

        private static void Analyze(EnumerationDeclaration enumeration, Scope scope)
        {
            enumeration.Type = Analyze(enumeration.Type, scope);
            if (!(enumeration.Type is PrimitiveType) || enumeration.Type is PrimitiveType primitive && primitive.Tag > PrimitiveTypeTag.U64)
                Report.Error($"{enumeration.Name.Start} Enumeration type is not an integer type.");

            var constantWithTheSameNameAsDeclaration = enumeration.Constants.FirstOrDefault(c => c.Name.Value == enumeration.Name.Value);
            if (constantWithTheSameNameAsDeclaration != null)
                Report.Error($"{constantWithTheSameNameAsDeclaration.Name.Start} Constants cannot have the same name as the enumeration.");

            foreach (var constant in enumeration.Constants)
            {
                foreach (var other in enumeration.Constants)
                {
                    if (constant == other)
                        continue;
                    else if (constant.Name.Value == other.Name.Value)
                        Report.Error($"{other.Name.Start} Redeclares {other.Name.Value}.");
                }
            }

            var lastValue = 0;
            foreach (var constant in enumeration.Constants)
            {
                if (constant.Value == null)
                {
                    constant.Value = new Number()
                    {
                        Tag = (NumberTag)((PrimitiveType)enumeration.Type).Tag,
                        Value = (lastValue++).ToString(),
                        Type = enumeration.Type
                    };
                }
                else if (!Compare(enumeration.Type, Analyze(constant.Value, enumeration.Type, scope)))
                    Report.Error($"{constant.Name.Start} Enumeration is {enumeration.Type} but constant {constant.Name.Value} is {constant.Value.Type}.");
            }
        }

        private static void Analyze(List<CompoundStatement> propositions, Scope scope)
        {
            foreach (var proposition in propositions)
            {
                proposition.Scope.Parent = scope;
                Analyze(proposition, null, proposition.Scope);
            }
        }

        private static void Analyze(Statement statement, Type output, Scope parent)
        {
            if (statement is CompoundStatement c)
            {
                foreach (var s in c.Statements)
                    Analyze(s, output, parent);
            }
            else if (statement is AssignmentStatement assignment)
            {
                var locals = CountLocals(parent);
                void Mismatch(int left, int right) =>
                    Report.Error($"{assignment.Operator.Start} Assignment mismatch of {left} variable{(left > 1 ? "s" : "")} and {right} output{(right > 1 ? "s" : "")}.");

                Expression Assign(Expression left, Type rightType, Position right)
                {
                    if (left is Identifier reference)
                    {
                        var leftType = NullableLookup(reference, parent);
                        if (leftType == null)
                        {
                            if (leftType == null && rightType == null)
                                Report.Error($"{right} Cannot infer type of expression.");

                            if (assignment.Operator.Tag != OperatorTag.Equal)
                                Report.Error($"{reference.Start} Undefined reference to {reference.Value}.");
                            else
                            {
                                left = new VariableDeclaration()
                                {
                                    Type = rightType,
                                    Variable = reference
                                };

                                parent.Symbols.Add(new LocalSymbol()
                                {
                                    Name = reference.Value,
                                    Type = rightType,
                                    Index = locals++
                                });
                            }
                        }
                        else if (!(leftType is Pointer && rightType == null) && !Compare(leftType, rightType))
                            Report.Error($"{right} Expected {leftType} but found {rightType}.");
                    }
                    else if (left.Type == null)
                        Report.Error($"{assignment.Operator.Start} Null can't be assigned to.");
                    else if (!Compare(left.Type, rightType))
                        Report.Error($"{left.Start} The expression is not addressable.");

                    var operatorUsedOnNonInteger = (assignment.Operator.Tag == OperatorTag.BitwiseAndEqual ||
                                                    assignment.Operator.Tag == OperatorTag.BitwiseOrEqual ||
                                                    assignment.Operator.Tag == OperatorTag.BitwiseXorEqual ||
                                                    assignment.Operator.Tag == OperatorTag.LeftShiftEqual ||
                                                    assignment.Operator.Tag == OperatorTag.RightShiftEqual) &&
                                                  (!(rightType is PrimitiveType) ||
                                                  ((PrimitiveType)rightType).Tag > PrimitiveTypeTag.U64);

                    if (operatorUsedOnNonInteger || !(rightType is PrimitiveType) && assignment.Operator.Tag != OperatorTag.Equal)
                        Report.Error($"{assignment.Operator.Start} This operator is only defined for integer types.");

                    return left;
                }

                for (var r = 0; r < assignment.Right.Count; r++)
                {
                    var rightType = default(Type);
                    if (assignment.Right[r] is Call call)
                    {
                        rightType = Analyze(assignment.Right[r], null, parent);
                        if (rightType is TupleType tuple)
                        {
                            if (assignment.Right.Count != 1)
                                Report.Error($"{call.Start} A Call outputting multiple values must be the only expression on the right hand side.");
                            else if (assignment.Left.Count != tuple.Types.Count)
                                Mismatch(assignment.Left.Count, tuple.Types.Count);
                            else
                            {
                                for (var l = 0; l < assignment.Left.Count; l++)
                                    assignment.Left[l] = Assign(assignment.Left[l], tuple.Types[l], call.Start);
                            }

                            break;
                        }
                    }
                    else if (assignment.Left.Count != assignment.Right.Count)
                        Mismatch(assignment.Left.Count, assignment.Right.Count);
                    else if (rightType == null && assignment.Left[r] is Identifier reference)
                    {
                        rightType = NullableLookup(reference, parent);
                        if (rightType == null)
                            rightType = Analyze(assignment.Right[r], null, parent);
                        else
                            rightType = Analyze(assignment.Right[r], rightType, parent);
                    }
                    else
                        rightType = Analyze(assignment.Right[r], Analyze(assignment.Left[r], null, parent), parent);

                    assignment.Left[r] = Assign(assignment.Left[r], rightType, assignment.Right[r].Start);
                }
            }
            else if (statement is ExpressionStatement e)
            {
                Analyze(e.Expression, null, parent);
                if (!(e.Expression is Call || e.Expression is PostfixIncrement))
                    Report.Error($"{e.Expression.Start} Call or pre/post increment/decrement expression expected.");
            }
            else if (statement is IfStatement ifStatement)
            {
                for (var i = 0; i < ifStatement.Blocks.Count; i++)
                {
                    var block = ifStatement.Blocks[i];
                    if (i == 0)
                        block.Body.Scope.Parent = parent;
                    else
                        block.Body.Scope.Parent = ifStatement.Blocks[i - 1].Body.Scope;

                    if (block.Assignment != null)
                        Analyze(block.Assignment, output, block.Body.Scope);

                    if (block.Condition != null)
                    {
                        block.Condition.Type = Analyze(block.Condition, null, block.Body.Scope);
                        if (!Compare(block.Condition.Type, Bool))
                            Report.Error($"{block.Condition.Start} Bool expression expected.");
                    }

                    Analyze(block.Body, output, block.Body.Scope);
                }
            }
            else if (statement is ForStatement forStatement)
            {
                forStatement.Body.Scope.Parent = parent;
                Analyze(forStatement.Assignment, output, forStatement.Body.Scope);

                forStatement.Condition.Type = Analyze(forStatement.Condition, null, forStatement.Body.Scope);
                if (!Compare(forStatement.Condition.Type, Bool))
                    Report.Error($"{forStatement.Condition.Start} Bool expression expected.");

                foreach (var p in forStatement.Post)
                    Analyze(p, null, forStatement.Body.Scope);
            }
            else if (statement is WhileStatement whileStatement)
            {
                whileStatement.Body.Scope.Parent = parent;
                if (whileStatement.Assignment != null)
                    Analyze(whileStatement.Assignment, output, whileStatement.Body.Scope);

                whileStatement.Condition.Type = Analyze(whileStatement.Condition, null, whileStatement.Body.Scope);
                if (!Compare(whileStatement.Condition.Type, Bool))
                    Report.Error($"{whileStatement.Condition.Start} Bool expression expected.");

                Analyze(whileStatement.Body, output, whileStatement.Body.Scope);
            }
            else if (statement is ReturnStatement returnStatement)
            {
                var expressionsAndOutputIsNotBalanced = false;
                if (output is TupleType t)
                    expressionsAndOutputIsNotBalanced = returnStatement.Expressions.Count != t.Types.Count;
                else if (output == null && returnStatement.Expressions.Count != 0 || output != null && returnStatement.Expressions.Count != 1)
                    expressionsAndOutputIsNotBalanced = true;

                if (expressionsAndOutputIsNotBalanced)
                    Report.Error($"{returnStatement.EndOfKeyword} The amount of return values doesn't match the output.");

                if (output is TupleType tuple)
                {
                    for (var i = 0; i < tuple.Types.Count; i++)
                    {
                        var expressionType = Analyze(returnStatement.Expressions[i], tuple.Types[i], parent);
                        if (expressionType != tuple.Types[i])
                            Report.Error($"{returnStatement.Expressions[i].Start} Expected {tuple.Types[i]} but found {expressionType}.");
                    }
                }
                else if (output != null)
                {
                    var expressionType = Analyze(returnStatement.Expressions[0], output, parent);
                    if (expressionType != output)
                        Report.Error($"{returnStatement.Expressions[0].Start} Expected {output} but found {expressionType}.");
                }
            }
            else if (statement is AssertStatement assert)
            {
                var type = Analyze(assert.Assertion, null, parent);
                if (!Compare(type, Bool))
                    Report.Error($"{assert.Assertion.Start} {Bool} expression expected.");
            }
            else if (statement is BreakStatement)
                ;
            else
                Report.Error($"Cannot analyze {statement.GetType().Name}.");
        }

        public static Type Analyze(Expression expression, Type expectedType, Scope scope)
        {
            if (expression is BinaryExpression binary)
            {
                var leftType = Analyze(binary.Left, null, scope);
                var rightType = Analyze(binary.Right, leftType, scope);

                binary.Type = Bool;
                if (!Compare(leftType, rightType))
                    Report.Error($"{binary.Right.Start} Expected {leftType} but found {rightType}.");
                else if (binary.Operator.Tag == OperatorTag.LogicalOr)
                {
                    if (!Compare(leftType, Bool))
                        Report.Error($"{binary.Operator.Start} This operator is only defined for Bool operands.");
                    else
                        binary.Type = Bool;
                }
                
                return binary.Type;
            }
            else if (expression is Boolean b)
            {
                b.Type = Bool;

                return Bool;
            }
            else if (expression is Number number)
            {
                if (number.Tag == NumberTag.ToBeInfered)
                {
                    if (expectedType is PrimitiveType primitive)
                    {
                        number.Tag = (NumberTag)primitive.Tag;
                        number.Type = primitive;
                    }
                    else
                        return null;
                }
                else
                    number.Type = NullableLookup(number.Tag.ToString(), scope, number.Start);

                return number.Type;
            }
            else if (expression is Identifier reference)
            {
                reference.Type = Lookup(reference, scope);

                return reference.Type;
            }
            else if (expression is Call call)
            {
                if (call.Reference is Identifier functionName)
                {
                    var functions = new List<FunctionDeclaration>();
                    var s = scope;

                    while (s != null)
                    {
                        foreach (var symbol in s.Symbols)
                        {
                            if (symbol.Type is FunctionDeclaration function && function.Name.Value == functionName.Value)
                                functions.Add(function);
                        }

                        s = s.Parent;
                    }

                    if (functions.Count == 0)
                        Report.Error($"{call.Reference.Start} Function expected.");

                    foreach (var input in call.Arguments)
                    {
                        if (!(input is Number n && n.Tag == NumberTag.ToBeInfered))
                            Analyze(input, null, scope);
                    }

                    var matches = new List<FunctionDeclaration>();
                    foreach (var function in functions)
                    {
                        if (function.Input.Count == call.Arguments.Count)
                        {
                            var isMatch = true;
                            for (var i = 0; i < call.Arguments.Count; i++)
                            {
                                bool ContainsGenericParameter(Type t)
                                {
                                    if (t is UnresolvedType unresolved)
                                        return function.Generalized.Any(g => g.Value == unresolved.Identifier.Value);
                                    else if (t is Pointer pointer)
                                        return ContainsGenericParameter(pointer.To);
                                    else
                                        return false;
                                }

                                var inputMatches = ContainsGenericParameter(function.Input[i].Type) ||
                                                   function.Input[i].Type is PrimitiveType pf &&
                                                   call.Arguments[i] is Number pn &&
                                                   pf.Tag != PrimitiveTypeTag.Bool &&
                                                   pn.Tag == NumberTag.ToBeInfered ||
                                                   Compare(function.Input[i].Type, call.Arguments[i].Type);

                                if (!inputMatches)
                                    isMatch = false;
                            }

                            if (isMatch)
                                matches.Add(function);
                        }
                    }

                    if (matches.Count == 1)
                    {
                        if (matches[0].Generalized.Count != 0)
                            matches[0] = Resolve(matches[0], call.Generics, call.Arguments, call.Start, matches[0].Resolved);
                        else if (call.Generics.Count != 0 && matches[0].Generalized.Count == 0)
                            Report.Error($"{call.Start} {matches[0].Name.Value} is not generic.");

                        for (var i = 0; i < call.Arguments.Count; i++)
                        {
                            if (call.Arguments[i] is Number n && n.Tag == NumberTag.ToBeInfered)
                                Analyze(call.Arguments[i], matches[0].Input[i].Type, scope);
                        }

                        call.Declaration = matches[0];
                        call.Type = matches[0].Output;

                        return call.Type;
                    }
                    else if (matches.Count > 1)
                        Report.Error($"{call.Reference.Start} Ambiguous call between:{Environment.NewLine}{string.Join(Environment.NewLine, matches.Select(f => $"{f.Name.Start} {f.Name.Value}({string.Join(", ", f.Input.Select(i => i.Type.ToString()))})"))}");
                    else
                        Report.Error($"{call.Reference.Start} Function expected.");
                }
                else
                    Report.Error($"{call.Reference.Start} Calling {call.Reference.GetType().Name} not yet supported.");
            }
            else if (expression is AddressOf ao)
            {
                ao.Type = new Pointer()
                {
                    To = Analyze(ao.Expression, null, scope)
                };

                return ao.Type;
            }
            else if (expression is Dereference dereference)
            {
                if (Analyze(dereference.Expression, null, scope) is Pointer pointer)
                    return dereference.Type = pointer.To;
                else
                    Report.Error($"{dereference.Start} Cannot dereference {dereference.Expression.Type}.");
            }
            else if (expression is DotExpression dot)
            {
                var typeOfStructure = dot.Type ?? Analyze(dot.Structure, null, scope);
                if (typeOfStructure is EnumerationDeclaration enumeration)
                {
                    if (dot.Field is Identifier constant)
                    {
                        if (!enumeration.Constants.Any(c => c.Name.Value == constant.Value))
                            Report.Error($"{constant.Start} {constant.Value} is not a constant in {enumeration.Name.Value}.");
                        else
                            return enumeration.Type;
                    }
                    else
                        Report.Error($"{dot.Field.Start} Identifier expected.");
                }
                else if (typeOfStructure is CompoundDeclaration compound)
                {
                    if (dot.Field is Identifier field)
                    {
                        dot.Type = compound.Fields.FirstOrDefault(f => f.Name.Value == field.Value).Type;
                        if (dot.Type == null)
                            Report.Error($"{field.Start} {field.Value} is not a field in {compound.Name.Value}.");
                        else
                            return dot.Type;
                    }
                    else if (dot.Field is DotExpression d)
                    {
                        if (d.Structure is Identifier f)
                        {
                            d.Type = compound.Fields.FirstOrDefault(a => a.Name.Value == f.Value).Type;
                            d.Type = Analyze(d, null, scope);

                            return d.Type;
                        }
                    }
                }
                else
                    Report.Error($"{expression.Start} Cannot analyze {expression.GetType().Name}.");
            }
            else if (expression is CompoundLiteral literal)
            {
                literal.Type = Lookup(literal.Structure, scope);
                if (literal.Type is CompoundDeclaration compound)
                {
                    foreach (var initializer in literal.Initializers)
                    {
                        var field = compound.Fields.FirstOrDefault(f => f.Name.Value == initializer.Name.Value);
                        if (field == null)
                            Report.Error($"{initializer.Name.Start} Undefined reference to {initializer.Name.Value}.");
                        else
                        {
                            initializer.Value.Type = Analyze(initializer.Value, field.Type, scope);
                            if (!Compare(field.Type, initializer.Value.Type))
                                Report.Error($"{initializer.Value.Start} Expected {field.Type} but found {initializer.Value.Type}.");
                        }
                    }

                    return compound;
                }
                else if (literal.Type != null)
                    Report.Error($"{expression.Start} Compound expected");
            }
            else if (expression is PostfixIncrement pfi)
            {
                pfi.Type = Analyze(pfi.Expression, null, scope);
                if (pfi.Expression is Identifier r && r.Type is PrimitiveType primitive && primitive.Tag < PrimitiveTypeTag.Bool)
                    return pfi.Type;
                else
                    Report.Error($"{pfi.Expression.Start} Reference to number typed variable expected.");
            }
            else if (expression is SizeOf sizeOf)
            {
                sizeOf.TypeBeingSizedUp = Lookup(((UnresolvedType)sizeOf.TypeBeingSizedUp).Identifier, scope);
                sizeOf.Type = I32;

                return sizeOf.Type;
            }
            else if (expression is Null)
                return null;
            else
                Report.Error($"{expression.Start} Cannot analyze {expression.GetType().Name}.");

            return null;
        }

        private static Type Analyze(Type type, Scope scope)
        {
            if (type is Pointer pointer)
            {
                pointer.To = Analyze(pointer.To, scope);

                return pointer;
            }
            else if (type is UnresolvedType unresolved)
                return Lookup(unresolved.Identifier, scope);
            else
                return type;
        }

        private static Type Lookup(Identifier name, Scope scope)
        {
            var type = NullableLookup(name, scope);
            if (type == null)
                Report.Error($"{name.Start} Undefined reference to {name.Value}.");

            return type;
        }

        private static void Lookup(string name, Scope scope, List<Type> types)
        {
            foreach (var symbol in scope.Symbols)
            {
                if (symbol.Name == name)
                    types.Add(symbol.Type);
            }

            if (scope.Parent != null)
                Lookup(name, scope.Parent, types);
        }

        private static Type NullableLookup(Identifier name, Scope scope)
        {
            return NullableLookup(name.Value, scope, name.Start);
        }

        private static Type NullableLookup(string name, Scope scope, Position position)
        {
            var types = new List<Type>();

            Lookup(name, scope, types);
            if (types.Count == 0)
                return null;
            else if (types.Count > 1)
            {
                var builder = new StringBuilder();
                builder.AppendLine($"{position} Ambiguous reference to:");

                foreach (var type in types)
                {
                    if (type is PrimitiveType primitive)
                        builder.AppendLine(primitive.Tag.ToString());
                    else if (type is EnumerationDeclaration enumeration)
                        builder.AppendLine($"{enumeration.Name.Start} enumeration {enumeration.Name.Value}");
                    else if (type is CompoundDeclaration compound)
                        builder.AppendLine($"{compound.Name.Start} {compound.Tag} {compound.Name.Value}");
                    else if (type is FunctionDeclaration function)
                        builder.AppendLine($"{function.Name.Start} function {function.Name.Value}");
                }

                Report.Error(builder.ToString());
            }

            return types[0];
        }

        private static FunctionDeclaration Resolve(FunctionDeclaration generic, List<Type> explicits, List<Expression> input, Position callSite, List<FunctionDeclaration> cache)
        {
            var resolved = new FunctionDeclaration();
            resolved.Name = generic.Name;
            resolved.Body = new CompoundStatement();
            resolved.Body.Scope = generic.Body.Scope.Parent;
            resolved.IsPublic = generic.IsPublic;
            resolved.IsExternal = generic.IsExternal;
            resolved.Library = generic.Library;

            var genericToType = new Dictionary<string, Type>();
            var types = default(List<Type>);
            if (explicits.Count != 0)
            {
                if (explicits.Count > generic.Generalized.Count)
                    Report.Error($"{callSite} To many types specified.");
                else if (explicits.Count < generic.Generalized.Count)
                    Report.Error($"{callSite} To few types specified.");
                else
                {
                    types = explicits;
                    for (var i = 0; i < explicits.Count; i++)
                        genericToType.Add(generic.Generalized[i].Value, explicits[i]);
                }
            }
            else
            {
                types = new List<Type>();
                for (var i = 0; i < generic.Input.Count; i++)
                {
                    // copy the nested type.
                    Type Copy(Type genericType, Type inputType)
                    {
                        if (genericType is UnresolvedType unresolved)
                        {
                            if (generic.Generalized.Any(g => g.Value == unresolved.Identifier.Value))
                            {
                                if (!genericToType.ContainsKey(unresolved.Identifier.Value))
                                {
                                    if (input[i].Type == null)
                                        Report.Error($"{callSite} Specify all generics since not all can be inferred.");

                                    genericToType.Add(unresolved.Identifier.Value, input[i].Type);
                                    return inputType;
                                }
                                else if (inputType == null)
                                    return genericToType[unresolved.Identifier.Value];
                                else
                                    return inputType;
                            }
                        }
                        else if (genericType is Pointer pa && inputType is Pointer pb)
                            return new Pointer()
                            {
                                To = Copy(pa.To, pb.To)
                            };
                        
                        throw new NotImplementedException(genericType.GetType().Name);
                    }

                    types.Add(Copy(generic.Input[i].Type, input[i].Type));
                }
            }

            if (!generic.Generalized.All(g => genericToType.ContainsKey(g.Value)))
                Report.Error($"{callSite} Specify all generics since not all can be inferred.");

            Type Resolve(Type t)
            {
                if (t is UnresolvedType u)
                    return genericToType[u.Identifier.Value];
                else if (t is Pointer pointer)
                    return new Pointer()
                    {
                        To = Resolve(pointer.To)
                    };
                else
                    return t;
            }

            for (var i = 0; i < generic.Input.Count; i++)
            {
                bool HasUnresolvedType(Type t)
                {
                    if (t is UnresolvedType)
                        return true;
                    else if (t is Pointer pointer)
                        return HasUnresolvedType(pointer.To);

                    return false;
                }

                if (HasUnresolvedType(generic.Input[i].Type))
                {
                    resolved.Input.Add(new Argument()
                    {
                        Name = generic.Input[i].Name,
                        Type = types[i]
                    });
                }
                else
                    resolved.Input.Add(generic.Input[i]);
            }

            if (generic.Output is TupleType tuple)
            {
                var output = new TupleType();
                output.Types = new List<Type>();

                for (var i = 0; i < tuple.Types.Count; i++)
                {
                    if (tuple.Types[i] is UnresolvedType unresolved && genericToType.ContainsKey(unresolved.Identifier.Value))
                        output.Types.Add(genericToType[unresolved.Identifier.Value]);
                    else
                        output.Types.Add(tuple.Types[i]);
                }

                resolved.Output = output;
            }
            else
                resolved.Output = Resolve(generic.Output);

            AnalyzeSignature(resolved.Body.Scope.Parent, resolved);
            foreach (var function in cache)
            {
                var match = true;
                for (var i = 0; i < function.Input.Count; i++)
                {
                    if (!Compare(resolved.Input[i].Type, function.Input[i].Type))
                    {
                        match = false;
                        break;
                    }
                }

                if (match)
                    return function;
            }

            resolved.Body = (CompoundStatement)ResolveStatement(generic.Body, generic.Body.Scope.Parent);
            AnalyzeBody(resolved);
            cache.Add(resolved);

            return resolved;
        }

        private static Statement ResolveStatement(Statement statement, Scope parent)
        {
            if (statement is CompoundStatement compound)
            {
                var resolved = new CompoundStatement();
                resolved.Scope.Parent = parent;

                foreach (var s in compound.Statements)
                    resolved.Statements.Add(ResolveStatement(s, resolved.Scope));

                return resolved;
            }
            else if (statement is ReturnStatement returnStatement)
                return new ReturnStatement()
                {
                    EndOfKeyword = returnStatement.EndOfKeyword,
                    Expressions = returnStatement.Expressions.Select(e => ResolveExpression(e)).ToList()
                };
            else
                Report.Error($"Cannot resolve {statement.GetType().Name}.");

            return null;
        }

        private static Expression ResolveExpression(Expression expression)
        {
            if (expression is Number number)
                return new Number
                {
                    Start = number.Start.Copy(),
                    Tag = number.Tag,
                    Type = number.Type,
                    Value = number.Value,
                    End = number.End.Copy()
                };
            else if (expression is Identifier reference)
                return new Identifier
                {
                    Start = reference.Start.Copy(),
                    Value = reference.Value,
                    End = reference.End.Copy()
                };
            else
                Report.Error($"Cannot resolve {expression.GetType().Name}.");

            return null;
        }

        private static bool Compare(Type a, Type b)
        {
            if (ReferenceEquals(a, b))
                return true;
            else if (a is PrimitiveType pa && b is PrimitiveType pb)
                return pa.Tag == pb.Tag;
            else if (a is Pointer ptra && b is Pointer ptrb)
                return Compare(ptra.To, ptrb.To);
            else if (a is TupleType ta && b is TupleType tb)
            {
                if (ta.Types.Count != tb.Types.Count)
                    return false;
                else
                {
                    for (var i = 0; i < ta.Types.Count; i++)
                    {
                        if (!Compare(ta.Types[i], tb.Types[i]))
                            return false;
                    }

                    return true;
                }
            }
            else
                return false;
        }

        private static ushort CountLocals(Scope scope)
        {
            ushort count = 0;
            foreach (var symbol in scope.Symbols)
            {
                if (symbol is LocalSymbol)
                    count++;
            }

            if (scope.Parent == null)
                return count;
            else
                return (ushort)(count + CountLocals(scope.Parent));
        }
    }
}