Public Class MachineDefinition

    Public Property ID As Integer
    Public Property Name As String
    Public Property TypeID As Integer
    Public Property CNC_Folder As String
    Public Property Clamp_Buffer As Double
    Public Property Clamp_Min_Split_Length As Double
    Public Property Clamp_Min_Split_Width As Double
    Public Property Holddown_Diameter As Double
    Public Property Holddown_Location_X1 As Double
    Public Property Holddown_Location_X2 As Double
    Public Property Holddown_Location_Y As Double
    Public Property Holddown_Type As Double
    Public Property Hole_Minus_Tolerance As Double
    Public Property Hole_Plus_Tolerance As Double
    Public Property Ind_Hits As Integer
    Public Property Max_Flow As Double
    Public Property Max_Horsepower As Double
    Public Property Max_Material_Length As Double
    Public Property Max_Material_Thickness As Double
    Public Property Max_Material_Weight As Double
    Public Property Max_Material_Width As Double
    Public Property Max_Pressure As Double
    Public Property Max_Tonnage As Double
    Public Property Max_Travel_Limit_X As Double
    Public Property Max_Travel_Limit_Y As Double
    Public Property Max_Travel_Limit_Z As Double
    Public Property Min_Area As Double
    Public Property Min_Travel_Limit_X As Double
    Public Property Min_Travel_Limit_Y As Double
    Public Property Min_Travel_Limit_Z As Double
    Public Property Number_of_Clamps As Integer
    Public Property Punch_Clamp_Deadzone_Center As Double
    Public Property Punch_Clamp_Deadzone_Length As Double
    Public Property Punch_Clamp_Deadzone_Width As Double
    Public Property Punch_Dropdoor_Location_X As Double
    Public Property Punch_Dropdoor_Location_Y As Double
    Public Property Punch_Dropdoor_Max_Length As Double
    Public Property Punch_Dropdoor_Max_Width As Double
    Public Property Punch_Dropdoor_Min_Length As Double
    Public Property Punch_Dropdoor_Min_Width As Double
    Public Property Scribe_With_Torch As Integer
    Public Property Space_Tolerance As Double
    Public Property Torch_Offset_In_X As Double
    Public Property Torch_Clamp_Deadzone_Center As Double
    Public Property Torch_Clamp_Deadzone_Length As Double
    Public Property Torch_Clamp_Deadzone_Width As Double
    Public Property Torch_Dropdoor_Location_X As Double
    Public Property Torch_Dropdoor_Location_Y As Double
    Public Property Torch_Dropdoor_Max_Length As Double
    Public Property Torch_Dropdoor_Max_Width As Double
    Public Property Torch_Dropdoor_Min_Length As Double
    Public Property Torch_Dropdoor_Min_Width As Double
    Public Property Quadrant As MachineEnvelope.MachineQuadrant
    Public Property Units As Integer
    Public Property WorkplaneTypeID As Integer
    Public Property Description As String

    ' User-selected configuration IDs
    Public Shared Property SelectedMachineID As Integer
    Public Shared Property SelectedToolSetupID As Integer
    Public Shared Property SelectedLayerSetupID As Integer
    Public Shared Property SelectedMaterialID As Integer

    ' Optional: Name fields for convenience
    Public Property MachineName As String
    Public Property ToolSetupName As String
    Public Property LayerSetupName As String
    Public Property MaterialName As String

End Class
