using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;

internal static class Semantics
{
    public static readonly PrimitiveType I8 = new PrimitiveType() { Tag = PrimitiveTypeTag.I8 };
    public static readonly PrimitiveType I16 = new PrimitiveType() { Tag = PrimitiveTypeTag.I16 };
    public static readonly PrimitiveType I32 = new PrimitiveType() { Tag = PrimitiveTypeTag.I32 };
    public static readonly PrimitiveType I64 = new PrimitiveType() { Tag = PrimitiveTypeTag.I64 };
    public static readonly PrimitiveType U8 = new PrimitiveType() { Tag = PrimitiveTypeTag.U8 };
    public static readonly PrimitiveType U16 = new PrimitiveType() { Tag = PrimitiveTypeTag.U16 };
    public static readonly PrimitiveType U32 = new PrimitiveType() { Tag = PrimitiveTypeTag.U32 };
    public static readonly PrimitiveType U64 = new PrimitiveType() { Tag = PrimitiveTypeTag.U64 };
    public static readonly PrimitiveType F32 = new PrimitiveType() { Tag = PrimitiveTypeTag.F32 };
    public static readonly PrimitiveType F64 = new PrimitiveType() { Tag = PrimitiveTypeTag.F64 };
    public static readonly PrimitiveType Bool = new PrimitiveType() { Tag = PrimitiveTypeTag.Bool };

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

        // Resolve name spaces.
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

        // Type check enumerations.
        foreach (var enumeration in program.Files.SelectMany(f => f.Enumerations))
        {
            enumeration.Type = Resolve(enumeration.Type, program.Scope);
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
                        Text = (lastValue++).ToString(),
                        Type = enumeration.Type
                    };

                    Analyze(constant.Value, null, program.Scope);
                }
                else if (!Compare(enumeration.Type, Analyze(constant.Value, enumeration.Type, program.Scope)))
                    Report.Error($"{constant.Name.Start} Enumeration is {enumeration.Type} but constant {constant.Name.Value} is {constant.Value.Type}.");
            }
        }

        foreach (var file in program.Files)
        {
            foreach (var compound in file.Compounds)
            {
                var fieldWithTheSameNameAsDeclaration = compound.Fields.FirstOrDefault(f => f.Name.Value == compound.Name.Value);
                if (fieldWithTheSameNameAsDeclaration != null)
                    Report.Error($"{fieldWithTheSameNameAsDeclaration.Name.Start} Fields cannot have the same name as the compound.");

                foreach (var field in compound.Fields)
                {
                    var positionOfType = ((UnresolvedType)field.Type).Name.Start;

                    field.Type = Resolve(field.Type, file.Scope);
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

        // something else happens from here on.
        foreach (var file in program.Files)
        {
            foreach (var function in file.Functions)
                AnalyzeSignature(file.Scope, function);
        }

        foreach (var function in program.Files.SelectMany(f => f.Functions))
            AnalyzeBody(function);

        foreach (var file in program.Files)
        {
            foreach (var proposition in file.Propositions)
            {
                proposition.Scope.Parent = program.Scope;
                Analyze(proposition, null, proposition.Scope);
            }
        }

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

    private static void AnalyzeSignature(Scope parent, FunctionDeclaration function)
    {
        bool IsGenericType(Type t)
        {
            if (t is UnresolvedType unresolved)
                return !function.Generalized.Any(g => unresolved.Name.Value == g.Value);
            else if (t is PointerType pointer)
                return IsGenericType(pointer.To);
            else if (t is PrimitiveType)
                return false;

            return true;
        }

        function.Body.Scope.Parent = parent;
        for (var i = 0; i < function.Input.Count; i++)
        {
            if (IsGenericType(function.Input[i].Type))
                function.Input[i].Type = Resolve(function.Input[i].Type, parent);
        }

        if (function.Output is TupleType tuple)
        {
            for (var i = 0; i < tuple.Types.Count; i++)
            {
                if (IsGenericType(tuple.Types[i]))
                    tuple.Types[i] = Resolve(tuple.Types[i], parent);
            }
        }
        else if (function.Output != null && IsGenericType(function.Output))
            function.Output = Resolve(function.Output, parent);
    }

    private static void AnalyzeBody(FunctionDeclaration function)
    {
        var index = default(ushort);
        foreach (var argument in function.Input)
        {
            if (NullableLookup(argument.Name.Value, function.Body.Scope, argument.Name.Start) is PrimitiveType primitive)
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

    private static void Analyze(Statement statement, Type output, Scope parent)
    {
        if (statement is CompoundStatement c)
        {
            foreach (var s in c.Statements)
                Analyze(s, output, parent);
        }
        else if (statement is AssignmentStatement assignment)
        {
            void Mismatch(int left, int right) =>
                Report.Error($"{assignment.Operator.Start} Assignment mismatch of {left} variable{(left > 1 ? "s" : "")} and {right} output{(right > 1 ? "s" : "")}.");

            Expression DeclareVariableOrAnalyze(Expression left, Type rightType, Position right)
            {
                if (left is Identifier reference)
                {
                    reference.Type = NullableLookup(reference.Value, parent, reference.Start);
                    if (reference.Type == null)
                    {
                        if (reference.Type == null && rightType == null)
                            Report.Error($"{right} Cannot infer type of expression.");

                        if (assignment.Operator.Tag != OperatorTag.Equal)
                            Report.Error($"{reference.Start} Undefined reference to {reference.Value}.");
                        else
                        {
                            left = new VariableDeclaration()
                            {
                                Type = rightType,
                                Variable = reference,
                                Start = reference.Start,
                                End = reference.End
                            };

                            parent.Symbols.Add(new LocalSymbol()
                            {
                                Name = reference.Value,
                                Type = rightType
                            });
                        }
                    }
                }
                else if (left.Type == null)
                    Analyze(left, rightType, parent);

                return left;
            }

            void CheckForIntegerOnlyOperators(Type type)
            {
                var operatorUsedOnNonInteger = (assignment.Operator.Tag == OperatorTag.BitwiseAndEqual ||
                                                assignment.Operator.Tag == OperatorTag.BitwiseOrEqual ||
                                                assignment.Operator.Tag == OperatorTag.BitwiseXorEqual ||
                                                assignment.Operator.Tag == OperatorTag.LeftShiftEqual ||
                                                assignment.Operator.Tag == OperatorTag.RightShiftEqual) &&
                                              (!(type is PrimitiveType) ||
                                              ((PrimitiveType)type).Tag > PrimitiveTypeTag.U64);

                if (operatorUsedOnNonInteger || !(type is PrimitiveType) && assignment.Operator.Tag != OperatorTag.Equal)
                    Report.Error($"{assignment.Operator.Start} This operator is only defined for integer types.");
            }

            void IsAddressable(Expression left)
            {
                if (left is Dereference dereference)
                    IsAddressable(dereference.Expression);
                else if (left is BinaryExpression binary &&
                        (binary.Operator.Tag == OperatorTag.Plus || binary.Operator.Tag == OperatorTag.Minus) &&
                        (binary.Left.Type is PointerType && binary.Right.Type is PrimitiveType primitive && primitive.Tag == PrimitiveTypeTag.U64))
                    ;
                else if (!(left is Identifier || left is VariableDeclaration || left is ArrayAccess || left is FieldAccess))
                    Report.Error($"{left.Start} The expression is not addressable.");
            }

            void IsCompatible(Expression left, Type right, Position positionOfTheRightHandExpression)
            {
                if (!Compare(left.Type, right) &&
                    !(left.Type is PointerType && right is Null) &&
                    !(left.Type is PointerType && right is PrimitiveType primitive && primitive.Tag == PrimitiveTypeTag.U64))
                    Report.Error($"{positionOfTheRightHandExpression} Expected {left.Type} but found {right}.");
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
                            {
                                assignment.Left[l] = DeclareVariableOrAnalyze(assignment.Left[l], tuple.Types[l], call.Start);
                                IsAddressable(assignment.Left[l]);
                                IsCompatible(assignment.Left[l], tuple.Types[l], call.Start);
                                CheckForIntegerOnlyOperators(rightType);
                            }
                        }

                        break;
                    }
                }
                else if (assignment.Left.Count != assignment.Right.Count)
                    Mismatch(assignment.Left.Count, assignment.Right.Count);
                else if (rightType == null && assignment.Left[r] is Identifier reference)
                    rightType = Analyze(assignment.Right[r], NullableLookup(reference.Value, parent, reference.Start), parent);
                else
                    rightType = Analyze(assignment.Right[r], Analyze(assignment.Left[r], null, parent), parent);

                assignment.Left[r] = DeclareVariableOrAnalyze(assignment.Left[r], rightType, assignment.Right[r].Start);
                IsAddressable(assignment.Left[r]);
                IsCompatible(assignment.Left[r], rightType, assignment.Right[r].Start);
                CheckForIntegerOnlyOperators(rightType);
            }
        }
        else if (statement is ExpressionStatement e)
        {
            Analyze(e.Expression, null, parent);
            if (!(e.Expression is Call))
                Report.Error($"{e.Expression.Start} Call expected.");
        }
        else if (statement is IfStatement ifStatement)
        {
            for (var i = 0; i < ifStatement.Branches.Count; i++)
            {
                var branch = ifStatement.Branches[i];
                if (i == 0)
                    branch.Body.Scope.Parent = parent;
                else
                    branch.Body.Scope.Parent = ifStatement.Branches[i - 1].Body.Scope;

                if (branch.Assignment != null)
                    Analyze(branch.Assignment, output, branch.Body.Scope);

                if (branch.Condition != null)
                {
                    branch.Condition.Type = Analyze(branch.Condition, null, branch.Body.Scope);
                    if (!Compare(branch.Condition.Type, Bool))
                        Report.Error($"{branch.Condition.Start} Bool expression expected.");
                }

                Analyze(branch.Body, output, branch.Body.Scope);
            }
        }
        else if (statement is ForStatement forStatement)
        {
            forStatement.Body.Scope.Parent = parent;
            Analyze(forStatement.Assignment, output, forStatement.Body.Scope);

            forStatement.Condition.Type = Analyze(forStatement.Condition, null, forStatement.Body.Scope);
            if (!Compare(forStatement.Condition.Type, Bool))
                Report.Error($"{forStatement.Condition.Start} Bool expression expected.");

            Analyze(forStatement.Post, output, forStatement.Body.Scope);
            Analyze(forStatement.Body, output, forStatement.Body.Scope);
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
                Report.Error($"{returnStatement.Start} The amount of return values doesn't match the output.");

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
        else if (statement is AssertStatement assertStatement)
        {
            var type = Analyze(assertStatement.Assertion, null, parent);
            if (!Compare(type, Bool))
                Report.Error($"{assertStatement.Assertion.Start} {Bool} expression expected.");
        }
        else if (statement is BreakStatement)
            ;
        else
            Report.InternalError($"Cannot analyze {statement.GetType().Name}.");
    }

    public static Type Analyze(Expression expression, Type expectedType, Scope scope, Type previous = null)
    {
        if (expression is BinaryExpression binary)
        {
            var leftType = Analyze(binary.Left, expectedType, scope);
            var rightType = Analyze(binary.Right, leftType, scope);

            if (!Compare(leftType, rightType) && !(leftType is PointerType && rightType == U64))
                Report.Error($"{binary.Right.Start} Expected {leftType} but found {rightType}.");
            else if (binary.Operator.Tag == OperatorTag.LogicalOr || binary.Operator.Tag == OperatorTag.LogicalAnd)
            {
                if (leftType == Bool)
                    binary.Type = Bool;
                else
                    Report.Error($"{binary.Operator.Start} This operator is only defined for Bool operands.");
            }
            else if (binary.Operator.Tag == OperatorTag.EqualEqual || binary.Operator.Tag == OperatorTag.NotEqual)
            {
                if (leftType is PrimitiveType || leftType is PointerType || leftType is EnumerationDeclaration)
                    binary.Type = Bool;
                else
                    Report.Error($"{binary.Operator.Start} This operator is only defined for primtive and pointer operands.");
            }
            else if (binary.Operator.Tag >= OperatorTag.LessThanOrEqual && binary.Operator.Tag <= OperatorTag.GreaterThan)
            {
                if (leftType is PrimitiveType primitive && primitive.Tag != PrimitiveTypeTag.Bool || leftType is PointerType || leftType is EnumerationDeclaration)
                    binary.Type = Bool;
                else
                    Report.Error($"{binary.Operator.Start} This operator is only defined for number, pointer or enumeration operands.");
            }
            else if (binary.Operator.Tag >= OperatorTag.Plus && binary.Operator.Tag <= OperatorTag.BitwiseXor)
            {
                if (leftType is PrimitiveType p1 && (p1.Tag == PrimitiveTypeTag.F32 || p1.Tag == PrimitiveTypeTag.F64) && binary.Operator.Tag == OperatorTag.BitwiseXor)
                    Report.Error($"{binary.Operator.Start} This operator is only defined for number and pointer operands.");
                else if (leftType is PrimitiveType p2 && p2.Tag != PrimitiveTypeTag.Bool)
                    binary.Type = leftType;
                else if (leftType is PointerType && rightType is PrimitiveType p3 && p3.Tag != PrimitiveTypeTag.Bool)
                    binary.Type = leftType;
                else if (leftType is EnumerationDeclaration && (binary.Operator.Tag == OperatorTag.BitwiseOr || binary.Operator.Tag == OperatorTag.BitwiseXor))
                    binary.Type = leftType;
                else
                    Report.Error($"{binary.Operator.Start} This operator is only defined for number and pointer operands.");
            }
            else if (binary.Operator.Tag >= OperatorTag.Multiply && binary.Operator.Tag <= OperatorTag.RightShift)
            {
                if (binary.Operator.Tag >= OperatorTag.Multiply && binary.Operator.Tag <= OperatorTag.Modulo && leftType is PrimitiveType p1 && (p1.Tag == PrimitiveTypeTag.F32 || p1.Tag == PrimitiveTypeTag.F64))
                    binary.Type = leftType;
                else if (leftType is PrimitiveType p2 && p2.Tag != PrimitiveTypeTag.Bool)
                {
                    if ((p2.Tag == PrimitiveTypeTag.F32 || p2.Tag == PrimitiveTypeTag.F64) && (binary.Operator.Tag == OperatorTag.LeftShift || binary.Operator.Tag == OperatorTag.RightShift || binary.Operator.Tag == OperatorTag.BitwiseAnd || binary.Operator.Tag == OperatorTag.BitwiseXor))
                        Report.Error($"{binary.Operator.Start} This operator is only defined for number operands.");
                    else
                        binary.Type = leftType;
                }
                else if (binary.Operator.Tag == OperatorTag.BitwiseAnd && leftType is EnumerationDeclaration)
                    binary.Type = leftType;
                else
                    Report.Error($"{binary.Operator.Start} This operator is only defined for number operands.");
            }

            return binary.Type;
        }
        else if (expression is Boolean)
            return expression.Type = Bool;
        else if (expression is Number number)
        {
            if (number.Tag == NumberTag.ToBeInfered)
            {
                if (expectedType is PrimitiveType primitive)
                {
                    number.Tag = (NumberTag)primitive.Tag;
                    number.Type = primitive;
                }
                else if (expectedType is PointerType)
                {
                    number.Tag = NumberTag.U64;
                    number.Type = U64;
                }
                else
                    return null;
            }
            else
                number.Type = NullableLookup(number.Tag.ToString(), scope, number.Start);

            if (number.Tag == NumberTag.I8)
            {
                if (sbyte.TryParse(number.Text, out sbyte value))
                    number.Value = value;
                else
                    Report.Error($"{number.Start} {number.Text} overflows the {I8} type.");
            }
            else if (number.Tag == NumberTag.I16)
            {
                if (short.TryParse(number.Text, out short value))
                    number.Value = value;
                else
                    Report.Error($"{number.Start} {number.Text} overflows the {I16} type.");
            }
            else if (number.Tag == NumberTag.I32)
            {
                if (int.TryParse(number.Text, out int value))
                    number.Value = value;
                else
                    Report.Error($"{number.Start} {number.Text} overflows the {I32} type.");
            }
            else if (number.Tag == NumberTag.I64)
            {
                if (long.TryParse(number.Text, out long value))
                    number.Value = value;
                else
                    Report.Error($"{number.Start} {number.Text} overflows the {I64} type.");
            }
            else if (number.Tag == NumberTag.U8)
            {
                if (byte.TryParse(number.Text, out byte value))
                    number.Value = value;
                else
                    Report.Error($"{number.Start} {number.Text} overflows the {U8} type.");
            }
            else if (number.Tag == NumberTag.U16)
            {
                if (ushort.TryParse(number.Text, out ushort value))
                    number.Value = value;
                else
                    Report.Error($"{number.Start} {number.Text} overflows the {U16} type.");
            }
            else if (number.Tag == NumberTag.U32)
            {
                if (uint.TryParse(number.Text, out uint value))
                    number.Value = value;
                else
                    Report.Error($"{number.Start} {number.Text} overflows the {U32} type.");
            }
            else if (number.Tag == NumberTag.U64)
            {
                if (ulong.TryParse(number.Text, out ulong value))
                    number.Value = value;
                else
                    Report.Error($"{number.Start} {number.Text} overflows the {U64} type.");
            }
            else if (number.Tag == NumberTag.F32)
            {
                if (float.TryParse(number.Text, NumberStyles.AllowDecimalPoint, NumberFormatInfo.InvariantInfo, out float value))
                    number.Value = value;
                else
                    Report.Error($"{number.Start} {number.Text} overflows the {F32} type.");
            }
            else if (number.Tag == NumberTag.F64)
            {
                if (double.TryParse(number.Text, NumberStyles.AllowDecimalPoint, NumberFormatInfo.InvariantInfo, out double value))
                    number.Value = value;
                else
                    Report.Error($"{number.Start} {number.Text} overflows the {F64} type.");
            }
            else
                Report.InternalError($"Cannot type check {number.Tag}.");

            return number.Type;
        }
        else if (expression is Identifier identifier)
        {
            if (previous == null)
                return identifier.Type = Lookup(identifier, scope);
            else if (previous is CompoundDeclaration compoundDeclaration)
            {
                var field = compoundDeclaration.Fields.FirstOrDefault(f => f.Name.Value == identifier.Value);
                if (field == null)
                    Report.Error($"{identifier.Start} Undefined reference to {identifier.Value}.");

                return identifier.Type = field.Type;
            }
            else if (previous is EnumerationDeclaration enumerationDeclaration)
            {
                var constant = enumerationDeclaration.Constants.FirstOrDefault(f => f.Name.Value == identifier.Value);
                if (constant == null)
                    Report.Error($"{identifier.Start} Undefined reference to {identifier.Value}.");

                return identifier.Type = enumerationDeclaration.Type;
            }
            else if (previous is ArrayType arrayType)
            {
                if (identifier.Value == "capacity" || identifier.Value == "length")
                    return identifier.Type = U32;
                else if (identifier.Value == "data")
                    return identifier.Type = new PointerType() { To = arrayType.Of };
                else
                    Report.Error($"{identifier.Start} Undefined reference to {identifier.Value}.");
            }
            else
                Report.InternalError("How did you get here?");
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
                                    return function.Generalized.Any(g => g.Value == unresolved.Name.Value);
                                else if (t is PointerType pointer)
                                    return ContainsGenericParameter(pointer.To);
                                else if (t is PrimitiveType)
                                    return false;
                                else if (t is ArrayType arrayType)
                                    return ContainsGenericParameter(arrayType.Of);
                                else if (t is EnumerationDeclaration)
                                    return false;
                                else if (t is CompoundDeclaration compoundDeclaration)
                                    return false;
                                else
                                    Report.InternalError($"Cannot figure out if {t} contains a generic parameter.");

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
                    {
                        var resolved = Monomorphize(matches[0], call.Generics, call.Arguments, call.Start, matches[0].Resolved);
                        if (!matches[0].IsExternal)
                            matches[0] = resolved;

                        call.Type = resolved.Output;
                    }
                    else if (call.Generics.Count != 0 && matches[0].Generalized.Count == 0)
                        Report.Error($"{call.Start} {matches[0].Name.Value} is not generic.");
                    else
                        call.Type = matches[0].Output;

                    for (var i = 0; i < call.Arguments.Count; i++)
                    {
                        if (call.Arguments[i] is Number n && n.Tag == NumberTag.ToBeInfered)
                            Analyze(call.Arguments[i], matches[0].Input[i].Type, scope);
                    }

                    call.Declaration = matches[0];

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
        else if (expression is AddressOf addressOf)
        {
            return addressOf.Type = new PointerType()
            {
                To = Analyze(addressOf.Expression, null, scope)
            };
        }
        else if (expression is Dereference dereference)
        {
            if (Analyze(dereference.Expression, null, scope) is PointerType pointer)
                return dereference.Type = pointer.To;
            else
                Report.Error($"{dereference.Start} Cannot dereference {dereference.Expression.Type}.");
        }
        else if (expression is ArrayLiteral arrayLiteral)
        {
            arrayLiteral.ElementType = Resolve(arrayLiteral.ElementType, scope);
            return arrayLiteral.Type = new ArrayType()
            {
                Of = arrayLiteral.ElementType
            };
        }
        else if (expression is FieldAccess fieldAccess)
        {
            fieldAccess.Object.Type = Analyze(fieldAccess.Object, null, scope, previous);
            if (fieldAccess.Field is Identifier || fieldAccess.Field is FieldAccess)
                return fieldAccess.Type = fieldAccess.Field.Type = Analyze(fieldAccess.Field, null, scope, fieldAccess.Object.Type);
            
            Report.InternalError($"Cannot analyze field type {fieldAccess.Field.GetType().Name}.");
        }
        else if (expression is CompoundLiteral compoundLiteral)
        {
            compoundLiteral.Type = Lookup(compoundLiteral.Structure, scope);
            if (compoundLiteral.Type is CompoundDeclaration declaration)
            {
                foreach (var initializer in compoundLiteral.Initializers)
                {
                    var field = declaration.Fields.FirstOrDefault(f => f.Name.Value == initializer.Name.Value);
                    if (field == null)
                        Report.Error($"{initializer.Name.Start} Undefined reference to {initializer.Name.Value}.");

                    initializer.Value.Type = Analyze(initializer.Value, field.Type, scope);
                    if (!Compare(field.Type, initializer.Value.Type))
                        Report.Error($"{initializer.Value.Start} Expected {field.Type} but found {initializer.Value.Type}.");
                }

                return declaration;
            }
            else if (compoundLiteral.Type != null)
                Report.Error($"{expression.Start} Compound expected.");
        }
        else if (expression is SizeOf sizeOf)
        {
            sizeOf.TypeBeingSizedUp = Lookup(((UnresolvedType)sizeOf.TypeBeingSizedUp).Name, scope);
            sizeOf.Type = I32;

            return sizeOf.Type;
        }
        else if (expression is NullLiteral nullLiteral)
            return nullLiteral.Type = new Null();
        else if (expression is Not not)
        {
            not.Type = Bool;
            Analyze(not.Expression, null, scope);

            if (not.Expression.Type != Bool)
                Report.Error($"{not.Expression.Start} Bool expression expected.");

            return Bool;
        }
        else if (expression is Negate negate)
        {
            Analyze(negate.Expression, expectedType, scope);
            if (!(negate.Expression.Type is PrimitiveType primitive && (primitive.Tag < PrimitiveTypeTag.U8 || primitive.Tag == PrimitiveTypeTag.F32 || primitive.Tag == PrimitiveTypeTag.F64)))
                Report.Error($"{negate.Expression.Start} IXX or FXX expression expected.");
            else
                return negate.Type = negate.Expression.Type;
        }
        else if (expression is Cast cast)
        {
            Analyze(cast.Expression, null, scope);
            cast.To = Resolve(cast.To, scope);

            return cast.Type = cast.To;
        }
        else if (expression is ArrayAccess index)
        {
            Analyze(index.Position, U32, scope);
            if (index.Position.Type is PrimitiveType primitive && primitive.Tag == PrimitiveTypeTag.U32)
            {
                Analyze(index.Array, null, scope);
                if (index.Array.Type is ArrayType array)
                    return index.Type = new PointerType()
                    {
                        To = array.Of
                    };
                else
                    Report.Error($"{index.Position} Expected array but found {index.Array.Type}.");
            }
            else
                Report.Error($"{index.Position} Expected {U32} but found {index.Position.Type}.");
        }
        else
            Report.InternalError($"{expression.Start} Cannot analyze {expression.GetType().Name}.");

        return null;
    }

    private static Type Resolve(Type type, Scope scope)
    {
        if (type is PointerType pointer)
        {
            pointer.To = Resolve(pointer.To, scope);
            return pointer;
        }
        else if (type is ArrayType array)
        {
            array.Of = Resolve(array.Of, scope);
            return array;
        }
        else if (type is UnresolvedType unresolved)
            return Lookup(unresolved.Name, scope);
        else
            return type;
    }

    private static Type Lookup(Identifier name, Scope scope)
    {
        var type = NullableLookup(name.Value, scope, name.Start);
        if (type == null)
            Report.Error($"{name.Start} Undefined reference to {name.Value}.");

        return type;
    }

    private static Type Lookup(Name name, Scope scope)
    {
        var type = NullableLookup(name.Value, scope, name.Start);
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

    private static FunctionDeclaration Monomorphize(FunctionDeclaration generic, List<Type> explicits, List<Expression> input, Position callSite, List<FunctionDeclaration> cache)
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

            types = explicits;
            for (var i = 0; i < explicits.Count; i++)
                genericToType.Add(generic.Generalized[i].Value, explicits[i]);
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
                        if (generic.Generalized.Any(g => g.Value == unresolved.Name.Value))
                        {
                            if (!genericToType.ContainsKey(unresolved.Name.Value))
                            {
                                if (input[i].Type == null)
                                    Report.Error($"{callSite} Specify all generics since not all can be inferred.");

                                genericToType.Add(unresolved.Name.Value, inputType);
                                return inputType;
                            }
                            else if (inputType == null)
                                return genericToType[unresolved.Name.Value];
                            else
                                return inputType;
                        }
                    }
                    else if (genericType is PointerType pa && inputType is PointerType pb)
                        return new PointerType()
                        {
                            To = Copy(pa.To, pb.To)
                        };
                    else if (genericType is PrimitiveType)
                        return genericType;

                    throw new NotImplementedException(genericType.GetType().Name);
                }

                types.Add(Copy(generic.Input[i].Type, input[i].Type));
            }
        }

        if (!generic.Generalized.All(g => genericToType.ContainsKey(g.Value)))
            Report.Error($"{callSite} Specify all generics since not all can be inferred.");

        Type Monomorphize(Type t)
        {
            if (t is UnresolvedType u)
                return genericToType[u.Name.Value];
            else if (t is PointerType pointer)
                return new PointerType()
                {
                    To = Monomorphize(pointer.To)
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
                else if (t is PointerType pointer)
                    return HasUnresolvedType(pointer.To);
                else if (t is PrimitiveType)
                    return false;
                else
                    Report.InternalError($"Cannot check whether or not {t} is unresolved.");

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
                if (tuple.Types[i] is UnresolvedType unresolved && genericToType.ContainsKey(unresolved.Name.Value))
                    output.Types.Add(genericToType[unresolved.Name.Value]);
                else
                    output.Types.Add(tuple.Types[i]);
            }

            resolved.Output = output;
        }
        else
            resolved.Output = Monomorphize(generic.Output);

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

        resolved.Body = (CompoundStatement)Semantics.Monomorphize(generic.Body, generic.Body.Scope.Parent);
        AnalyzeBody(resolved);
        cache.Add(resolved);

        return resolved;
    }

    private static Statement Monomorphize(Statement statement, Scope parent)
    {
        if (statement is CompoundStatement compound)
        {
            var resolved = new CompoundStatement();
            resolved.Scope.Parent = parent;

            foreach (var s in compound.Statements)
                resolved.Statements.Add(Monomorphize(s, resolved.Scope));

            return resolved;
        }
        else if (statement is ReturnStatement returnStatement)
            return new ReturnStatement()
            {
                Start = returnStatement.Start,
                Expressions = returnStatement.Expressions.Select(e => Monomorphize(e)).ToList()
            };
        else
            Report.InternalError($"Cannot resolve {statement.GetType().Name}.");

        return null;
    }

    private static Expression Monomorphize(Expression expression)
    {
        if (expression is Number number)
            return new Number
            {
                Start = number.Start.Copy(),
                Tag = number.Tag,
                Type = number.Type,
                Text = number.Text
            };
        else if (expression is Identifier reference)
            return new Identifier
            {
                Start = reference.Start.Copy(),
                Value = reference.Value
            };
        else
            Report.InternalError($"Cannot resolve {expression.GetType().Name}.");

        return null;
    }

    private static bool Compare(Type a, Type b)
    {
        if (ReferenceEquals(a, b))
            return true;
        else if (a is PointerType ptra && b is PointerType ptrb)
            return Compare(ptra.To, ptrb.To);
        else if (a is ArrayType leftArray && b is ArrayType rightArray)
            return Compare(leftArray.Of, rightArray.Of);
        else if (a is TupleType ta && b is TupleType tb)
        {
            if (ta.Types.Count != tb.Types.Count)
                return false;

            for (var i = 0; i < ta.Types.Count; i++)
            {
                if (!Compare(ta.Types[i], tb.Types[i]))
                    return false;
            }

            return true;
        }
        else
            return false;
    }
}