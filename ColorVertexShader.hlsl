float4x4 ProjView;
float4 MasterCol;

void main(in  float4 Position : POSITION, in float4 Color : COLOR0,
          out float4 oPosition : SV_Position, out float4 oColor : COLOR0)
{
    oPosition = mul(ProjView, Position);
    oColor = MasterCol * Color;
}