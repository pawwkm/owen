using System;
using System.Collections.Generic;
using System.Linq;

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

            // Resolve namespaces.
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

            // Analyze enumerations.
            foreach (var enumeration in program.Files.SelectMany(f => f.Enumerations))
                Analyze(enumeration, program.Scope);

            // Analyze compounds.
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

            // Analyze recursive compounds.
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

            // Analyze function signatures.
            foreach (var file in program.Files)
            {
                foreach (var function in file.Functions)
                {
                    function.Body.Scope.Parent = file.Scope;
                    for (var i = 0; i < function.Input.Count; i++)
                        function.Input[i].Type = Analyze(function.Input[i].Type, file.Scope);

                    if (function.Output is TupleType tuple)
                    {
                        for (var i = 0; i < tuple.Types.Count; i++)
                            tuple.Types[i] = Analyze(tuple.Types[i], file.Scope);
                    }
                    else if (function.Output != null)
                        function.Output = Analyze(function.Output, file.Scope);
                }
            }

            // Analyze functions bodies.
            foreach (var function in program.Files.SelectMany(f => f.Functions))
            {
                var index = default(ushort);
                foreach (var argument in function.Input)
                {
                    if (NullableLookup(argument.Name.Value, function.Body.Scope) is PrimitiveType primitive)
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

                Analyze(function.Body, function.Output, function.Body.Scope);
            }

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
                        var leftType = NullableLookup(reference.Value, parent);
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
                        else if (!Compare(leftType, rightType))
                            Report.Error($"{right} Expected {leftType} but found {rightType}.");
                    }
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
                        rightType = NullableLookup(reference.Value, parent);
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
            else if (statement is ForStatement fs)
            {
                fs.Body.Scope.Parent = parent;
                Analyze(fs.Assignment, output, fs.Body.Scope);

                fs.Condition.Type = Analyze(fs.Condition, null, fs.Body.Scope);
                if (!Compare(fs.Condition.Type, Bool))
                    Report.Error($"{fs.Condition.Start} Bool expression expected.");

                foreach (var p in fs.Post)
                    Analyze(p, null, fs.Body.Scope);
            }
            else if (statement is WhileStatement w)
            {
                w.Body.Scope.Parent = parent;
                if (w.Assignment != null)
                    Analyze(w.Assignment, output, w.Body.Scope);

                w.Condition.Type = Analyze(w.Condition, null, w.Body.Scope);
                if (!Compare(w.Condition.Type, Bool))
                    Report.Error($"{w.Condition.Start} Bool expression expected.");

                Analyze(w.Body, output, w.Body.Scope);
            }
            else if (statement is ReturnStatement r)
            {
                var expressionsAndOutputIsNotBalanced = false;
                if (output is TupleType t)
                    expressionsAndOutputIsNotBalanced = r.Expressions.Count != t.Types.Count;
                else if (output == null && r.Expressions.Count != 0 || output != null && r.Expressions.Count != 1)
                    expressionsAndOutputIsNotBalanced = true;

                if (expressionsAndOutputIsNotBalanced)
                    Report.Error($"{r.EndOfKeyword} The amount of return values doesn't match the output.");

                if (output is TupleType tuple)
                {
                    for (var i = 0; i < tuple.Types.Count; i++)
                    {
                        var expressionType = Analyze(r.Expressions[i], tuple.Types[i], parent);
                        if (expressionType != tuple.Types[i])
                            Report.Error($"{r.Expressions[i].Start} Expected {tuple.Types[i]} but found {expressionType}.");
                    }
                }
                else if (output != null)
                {
                    var expressionType = Analyze(r.Expressions[0], output, parent);
                    if (expressionType != output)
                        Report.Error($"{r.Expressions[0].Start} Expected {output} but found {expressionType}.");
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

                binary.Type = new PrimitiveType()
                {
                    Tag = PrimitiveTypeTag.Bool
                };

                if (!Compare(leftType, rightType))
                    Report.Error($"{binary.Right.Start} Expected {leftType} but found {rightType}.");
                else
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
                    number.Type = NullableLookup(number.Tag.ToString(), scope);

                return number.Type;
            }
            else if (expression is Identifier reference)
            {
                reference.Type = Lookup(reference, scope);

                return reference.Type;
            }
            else if (expression is Call call)
            {
                if (Analyze(call.Reference, null, scope) is FunctionDeclaration function)
                {
                    if (call.Arguments.Count != function.Input.Count)
                        Report.Error($"{call.Reference.Start} No function overload fitting the given input.");

                    call.Declaration = function;
                    for (var i = 0; i < call.Arguments.Count; i++)
                    {
                        if (!Compare(function.Input[i].Type, Analyze(call.Arguments[i], function.Input[i].Type, scope)))
                            Report.Error($"{call.Reference.Start} No function overload fitting the given input.");
                    }

                    call.Type = function.Output;

                    return function.Output;
                }
                else
                    Report.Error($"{call.Reference.Start} Function expected.");
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

            throw new NotImplementedException($"Cannot analyze {expression.GetType().Name}.");
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
                Report.Error($"Cannot analyze {type.GetType().Name}.");

            return null;
        }

        private static Type Lookup(Identifier name, Scope scope)
        {
            var type = NullableLookup(name.Value, scope);
            if (type == null)
                Report.Error($"{name.Start} Undefined reference to {name.Value}.");

            return type;
        }

        private static Type NullableLookup(string name, Scope scope)
        {
            foreach (var symbol in scope.Symbols)
            {
                if (symbol.Name == name)
                    return symbol.Type;
            }

            if (scope.Parent == null)
                return null;
            else
                return NullableLookup(name, scope.Parent);
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