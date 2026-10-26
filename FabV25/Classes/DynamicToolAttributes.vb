Imports System.ComponentModel
Imports System.Drawing.Design
Imports System.Drawing
Public Class DynamicToolAttributes
    Implements ICustomTypeDescriptor


    Public _attributes As Dictionary(Of String, Object)
    Private _types As Dictionary(Of String, Type)

    Public Sub New(attributes As Dictionary(Of String, Object))
        _attributes = attributes
        _types = New Dictionary(Of String, Type)()

        ' Determine the property type based on the value
        For Each kvp In attributes
            If kvp.Value IsNot Nothing Then
                _types(kvp.Key) = kvp.Value.GetType()
            Else
                _types(kvp.Key) = GetType(Object)
            End If
        Next
    End Sub

    ' ---------------------------
    ' ICustomTypeDescriptor Implementation
    ' ---------------------------

    Public Function GetProperties() As PropertyDescriptorCollection Implements ICustomTypeDescriptor.GetProperties
        Return GetProperties(Nothing)
    End Function

    Public Function GetProperties(attributes() As Attribute) As PropertyDescriptorCollection Implements ICustomTypeDescriptor.GetProperties
        Dim props As New List(Of PropertyDescriptor)

        For Each kvp In _attributes
            props.Add(New DynamicPropertyDescriptor(kvp.Key, _types(kvp.Key)))
        Next

        Return New PropertyDescriptorCollection(props.ToArray())
    End Function

    Public Function GetAttributes() As AttributeCollection Implements ICustomTypeDescriptor.GetAttributes
        Return AttributeCollection.Empty
    End Function

    Public Function GetClassName() As String Implements ICustomTypeDescriptor.GetClassName
        Return Nothing
    End Function

    Public Function GetComponentName() As String Implements ICustomTypeDescriptor.GetComponentName
        Return Nothing
    End Function

    Public Function GetConverter() As TypeConverter Implements ICustomTypeDescriptor.GetConverter
        Return Nothing
    End Function

    Public Function GetDefaultEvent() As EventDescriptor Implements ICustomTypeDescriptor.GetDefaultEvent
        Return Nothing
    End Function

    Public Function GetDefaultProperty() As PropertyDescriptor Implements ICustomTypeDescriptor.GetDefaultProperty
        Return Nothing
    End Function

    Public Function GetEditor(editorBaseType As Type) As Object Implements ICustomTypeDescriptor.GetEditor
        Return Nothing
    End Function

    Public Function GetEvents() As EventDescriptorCollection Implements ICustomTypeDescriptor.GetEvents
        Return EventDescriptorCollection.Empty
    End Function

    Public Function GetEvents(attributes() As Attribute) As EventDescriptorCollection Implements ICustomTypeDescriptor.GetEvents
        Return EventDescriptorCollection.Empty
    End Function

    Public Function GetPropertyOwner(pd As PropertyDescriptor) As Object Implements ICustomTypeDescriptor.GetPropertyOwner
        Return Me
    End Function

    Default Public Property Item(propertyName As String) As Object
        Get
            If _attributes.ContainsKey(propertyName) Then
                Return _attributes(propertyName)
            End If
            Return Nothing
        End Get
        Set(value As Object)
            If _attributes.ContainsKey(propertyName) Then
                _attributes(propertyName) = value
            End If
        End Set
    End Property
End Class


Public Class CtgFileEditor
    Inherits UITypeEditor

    Public Overrides Function GetEditStyle(
        context As ITypeDescriptorContext
    ) As UITypeEditorEditStyle

        Return UITypeEditorEditStyle.Modal
    End Function

    Public Overrides Function EditValue(
        context As ITypeDescriptorContext,
        provider As IServiceProvider,
        value As Object
    ) As Object

        Using dialog As New OpenFileDialog()
            dialog.Title = "Select CTG Tool Definition"
            dialog.Filter = "CTG Files (*.ctg)|*.ctg"
            dialog.CheckFileExists = True
            dialog.Multiselect = False

            If value IsNot Nothing Then
                Dim currentPath As String = Convert.ToString(value)

                If IO.File.Exists(currentPath) Then
                    dialog.InitialDirectory = IO.Path.GetDirectoryName(currentPath)
                    dialog.FileName = IO.Path.GetFileName(currentPath)
                End If
            End If

            If dialog.ShowDialog() = DialogResult.OK Then
                Return dialog.FileName
            End If
        End Using

        Return value
    End Function
End Class

' ---------------------------
' Dynamic Property Descriptor
' ---------------------------

Public Class DynamicPropertyDescriptor
    Inherits PropertyDescriptor

    Private ReadOnly _propertyType As Type

    Public Sub New(name As String, propertyType As Type)
        MyBase.New(name, Nothing)

        If String.Equals(name, "Color", StringComparison.OrdinalIgnoreCase) Then
            _propertyType = GetType(Color)
        Else
            _propertyType = propertyType
        End If
    End Sub

    Public Overrides ReadOnly Property PropertyType As Type
        Get
            Return _propertyType
        End Get
    End Property

    Public Overrides ReadOnly Property ComponentType As Type
        Get
            Return GetType(DynamicToolAttributes)
        End Get
    End Property

    Public Overrides ReadOnly Property IsReadOnly As Boolean
        Get
            Select Case Name
                Case "Tool Crib.Description",
                     "Tool Crib.ID",
                     "Type ID",
                     "Workplanes.Description"
                    Return True

                Case Else
                    Return False
            End Select
        End Get
    End Property


    Public Overrides Function GetEditor(editorBaseType As Type) As Object
        If editorBaseType Is GetType(UITypeEditor) Then
            If String.Equals(Name, "Color", StringComparison.OrdinalIgnoreCase) Then
                Return New ColorEditor()
            End If

            If String.Equals(Name, "Filename", StringComparison.OrdinalIgnoreCase) Then
                Return New CtgFileEditor()
            End If
        End If

        Return MyBase.GetEditor(editorBaseType)
    End Function


    Public Overrides Function CanResetValue(component As Object) As Boolean
        Return False
    End Function

    Public Overrides Function GetValue(component As Object) As Object
        Dim value = CType(component, DynamicToolAttributes)(Name)

        If String.Equals(Name, "Color", StringComparison.OrdinalIgnoreCase) Then
            If value Is Nothing Then Return Color.White

            Try
                Return ColorTranslator.FromOle(Convert.ToInt32(value))
            Catch
                Return Color.White
            End Try
        End If

        Return value
    End Function

    Public Overrides Sub ResetValue(component As Object)
        ' Not implemented
    End Sub

    Public Overrides Sub SetValue(component As Object, value As Object)
        If String.Equals(Name, "Color", StringComparison.OrdinalIgnoreCase) Then
            Dim selectedColor As Color = CType(value, Color)
            CType(component, DynamicToolAttributes)(Name) =
                ColorTranslator.ToOle(selectedColor)
        Else
            CType(component, DynamicToolAttributes)(Name) = value
        End If
    End Sub

    Public Overrides Function ShouldSerializeValue(component As Object) As Boolean
        Return True
    End Function
End Class

