
Imports System.Data
Imports System.Globalization

Public Class HoleExpressionEvaluator

    Public Shared Function TryEvaluate(
        expression As String,
        ByRef result As Double) As Boolean

        result = 0

        If String.IsNullOrWhiteSpace(expression) Then
            Return False
        End If

        Try
            Dim table As New DataTable()

            Dim value As Object =
                table.Compute(expression, Nothing)

            If value Is Nothing OrElse
               value Is DBNull.Value Then
                Return False
            End If

            Dim numericResult As Double =
                Convert.ToDouble(
                    value,
                    CultureInfo.InvariantCulture)

            If Double.IsNaN(numericResult) OrElse
               Double.IsInfinity(numericResult) Then
                Return False
            End If

            result = numericResult
            Return True

        Catch
            Return False
        End Try

    End Function

End Class
