Imports System.ComponentModel


Public Class GeoLine
        Inherits GeoEntity

        <TypeConverter(GetType(ExpandableObjectConverter))>
        Public Property StartPoint As GeoPoint

        <TypeConverter(GetType(ExpandableObjectConverter))>
        Public Property EndPoint As GeoPoint

    End Class
