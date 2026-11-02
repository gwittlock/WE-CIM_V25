Imports System.ComponentModel

Public Class DynamicMachinePropertyBag
    Implements ICustomTypeDescriptor

    Private _properties As List(Of DynamicMachineProperty)
    Private ReadOnly _machine As Machine
    Public ReadOnly Property MachineRecord As Machine
        Get
            Return _machine
        End Get
    End Property

    Public Sub New(machine As Machine,
               attrs As List(Of MachineAttribute),
               types As List(Of MachineAttributeType))

        _machine = machine

        _properties = New List(Of DynamicMachineProperty)()

        For Each attr In attrs
            Dim typeDef = types.FirstOrDefault(
            Function(t) t.ID = attr.AttributeTypeID)

            If typeDef IsNot Nothing Then
                _properties.Add(
                New DynamicMachineProperty(attr, typeDef))
            End If
        Next

    End Sub



    Public Sub New(attrs As List(Of MachineAttribute), types As List(Of MachineAttributeType))
        _properties = New List(Of DynamicMachineProperty)()

        For Each attr In attrs
            Dim typeDef = types.FirstOrDefault(Function(t) t.ID = attr.AttributeTypeID)
            If typeDef IsNot Nothing Then
                _properties.Add(New DynamicMachineProperty(attr, typeDef))
            End If
        Next
    End Sub

    ' Indexer for convenience (optional)
    Default Public Property Item(name As String) As Object
        Get
            Dim prop = _properties.FirstOrDefault(Function(p) p.TypeDef.Display = name)
            Return If(prop?.Value, Nothing)
        End Get
        Set(value As Object)
            Dim prop = _properties.FirstOrDefault(Function(p) p.TypeDef.Display = name)
            If prop IsNot Nothing Then prop.Value = value
        End Set
    End Property

    Public Function GetProperties() As PropertyDescriptorCollection Implements ICustomTypeDescriptor.GetProperties

        Dim descriptors As New List(Of PropertyDescriptor)

        If _machine IsNot Nothing Then

            descriptors.Add(
        New CoreMachinePropertyDescriptor(
            "WorkplaneTypeID",
            "Quadrant"))

            descriptors.Add(
        New CoreMachinePropertyDescriptor(
            "Units",
            "Units"))

            descriptors.Add(
    New CoreMachinePropertyDescriptor(
        "CodePartProfile",
        "Code Part Profile"))

            descriptors.Add(
    New CoreMachinePropertyDescriptor(
        "CNC_Folder",
        "Default CNC Path"))


            descriptors.Add(
    New CoreMachinePropertyDescriptor(
        "CodeGenerationFile",
        "Code Generation File"))
        descriptors.Add(
    New CoreMachinePropertyDescriptor(
        "HoldDown_CTG",
        "HoldDown CTG"))

        descriptors.Add(
    New CoreMachinePropertyDescriptor(
        "Clamp_CTG",
        "Clamp CTG"))

        descriptors.Add(
    New CoreMachinePropertyDescriptor(
        "Table_CTG",
        "Table CTG"))


        End If


        ' Existing dynamic machine attributes
        For Each prop In _properties
            descriptors.Add(
            New DynamicMachinePropertyDescriptor(prop))
        Next

        Return New PropertyDescriptorCollection(descriptors.ToArray())

    End Function
    Public Function GetProperties(attributes() As Attribute) As PropertyDescriptorCollection Implements ICustomTypeDescriptor.GetProperties

        Return GetProperties()

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
End Class
