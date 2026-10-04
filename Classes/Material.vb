Imports System.IO
Imports IxMilia.Dxf
Imports Newtonsoft.Json

Public Class MaterialWrapper

    '-----------------------------
    ' Physical Properties
    '-----------------------------
    Public Class Material
        Public Shared Property X As Double
        Public Shared Property Y As Double
        Public Shared Property Width As Double
        Public Shared Property Height As Double
        Public Shared Property Thickness As Double
        Public Shared Property Color As Color = Color.LightGray

        Public Shared Function GetWidthValue(sheets As List(Of MaterialSheet), targetSheetId As Integer) As Object
            ' Use LINQ to find the sheet by ID, then find the parameter named "Width"
            Dim sheet = sheets.FirstOrDefault(Function(s) s.ID = targetSheetId)

            If sheet IsNot Nothing Then
                Dim widthParam = sheet.Values.FirstOrDefault(Function(p) p.Name.Equals("Width", StringComparison.OrdinalIgnoreCase))

                If widthParam IsNot Nothing Then
                    Return widthParam.Value
                End If
            End If

            Return Nothing
        End Function

    End Class
    Public Shared Current As New Material()

    '-----------------------------
    ' Job Space Origin (Fixed)
    '-----------------------------
    Public ReadOnly Property OriginX As Double
        Get
            Return 0
        End Get
    End Property

    Public ReadOnly Property OriginY As Double
        Get
            Return 0
        End Get
    End Property

End Class
