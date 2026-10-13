Imports System.Globalization
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models

Namespace ToolShapes

    Public Class ToolShapeFactory

        Public Shared Function Create(tool As Tool) As ToolShape

            If tool Is Nothing Then
                Throw New ArgumentNullException(NameOf(tool))
            End If

            If tool.Attributes Is Nothing Then
                Throw New ArgumentException(
                    "Tool.Attributes cannot be Nothing.",
                    NameOf(tool))
            End If

            Select Case tool.TypeID

                Case 15
                    Return New RoundToolShape(
        GetDouble(tool, "Diameter"))

                Case 16
                    Return New SquareToolShape(
        GetDouble(tool, "Width"),
        GetDouble(tool, "Corner Radius"))

                Case 17
                    Return New RectangularToolShape(
        GetDouble(tool, "Length"),
        GetDouble(tool, "Width"),
        GetDouble(tool, "Corner Radius"))

                Case 18
                    Return New ObroundToolShape(
        GetDouble(tool, "Width"),
        GetDouble(tool, "Length"))

                Case 19
                    Return New DiamondToolShape(
        GetDouble(tool, "Width"),
        GetDouble(tool, "Length"),
        GetDouble(tool, "Corner Radius"))

                Case 20
                    Return New CornerRadiusToolShape(
                        GetDouble(tool, "Radius"),
                        GetDouble(tool, "Web"),
                        GetDouble(tool, "Width"))

                Case 21
                    Return New SingleDToolShape(
                        GetDouble(tool, "Diameter"),
                        GetDouble(tool, "Width"))

                Case 22
                    Return New DoubleDToolShape(
                        GetDouble(tool, "Diameter"),
                        GetDouble(tool, "Width"))

                Case 23
                    Return New TrapezoidToolShape(
                        GetDouble(tool, "Length"),
                        GetDouble(tool, "Width"),
                        GetDouble(tool, "Angle"),
                        GetDouble(tool, "Corner Radius"))

                Case 24
                    Return New KeyholeToolShape(
                        GetDouble(tool, "Length"),
                        GetDouble(tool, "Major Diameter"),
                        GetDouble(tool, "Minor Diameter"))

                Case 27
                    Return New HexagonToolShape(
                        GetDouble(tool, "Diameter"))

                Case 29 ' Custom

                    Return CustomToolShape.FromFile(
                        GetString(tool, "Filename"),
                        GetDouble(tool, "Length"),
                        GetDouble(tool, "Width"))

                Case Else
                    Throw New NotSupportedException(
            "Tool Type ID " &
            tool.TypeID.ToString() &
            " is not supported by ToolShapeFactory.")

            End Select

        End Function
        Private Shared Function GetString(
    tool As Tool,
    fieldName As String) As String

            If tool Is Nothing Then
                Throw New ArgumentNullException(NameOf(tool))
            End If

            If tool.Attributes Is Nothing Then
                Throw New InvalidOperationException(
            "Tool.Attributes is Nothing.")
            End If

            Dim value As Object = Nothing

            Dim actualKey As String =
        tool.Attributes.Keys.FirstOrDefault(
            Function(k) String.Equals(
                k,
                fieldName,
                StringComparison.OrdinalIgnoreCase))

            If actualKey Is Nothing Then
                Return String.Empty
            End If

            value = tool.Attributes(actualKey)

            If value Is Nothing Then
                Return String.Empty
            End If

            Return Convert.ToString(
        value,
        CultureInfo.InvariantCulture)

        End Function

        Private Shared Function GetDouble(
    tool As Tool,
    fieldName As String) As Double

            If tool Is Nothing Then
                Throw New ArgumentNullException(NameOf(tool))
            End If

            If tool.Attributes Is Nothing Then
                Throw New InvalidOperationException(
            "Tool.Attributes is Nothing.")
            End If

            Dim actualKey As String =
        tool.Attributes.Keys.FirstOrDefault(
            Function(k) String.Equals(
                k,
                fieldName,
                StringComparison.OrdinalIgnoreCase))

            If actualKey Is Nothing Then
                Throw New ArgumentException(
            "Tool attribute '" &
            fieldName &
            "' was not found.",
            NameOf(tool))
            End If

            Dim value As Object =
        tool.Attributes(actualKey)

            If value Is Nothing Then
                Throw New ArgumentException(
            "Tool attribute '" &
            fieldName &
            "' is Nothing.",
            NameOf(tool))
            End If

            If TypeOf value Is Double Then
                Return DirectCast(value, Double)
            End If

            If TypeOf value Is Single Then
                Return CDbl(value)
            End If

            If TypeOf value Is Decimal Then
                Return CDbl(value)
            End If

            If TypeOf value Is Integer Then
                Return CDbl(value)
            End If

            If TypeOf value Is Long Then
                Return CDbl(value)
            End If

            If TypeOf value Is String Then

                Dim result As Double

                If Double.TryParse(
            DirectCast(value, String),
            NumberStyles.Float Or NumberStyles.AllowThousands,
            CultureInfo.InvariantCulture,
            result) Then

                    Return result

                End If

            End If

            Throw New ArgumentException(
        "Tool attribute '" &
        fieldName &
        "' could not be converted to Double.",
        NameOf(tool))

        End Function

    End Class

End Namespace