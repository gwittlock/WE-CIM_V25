
package Weng.CodeGen;

public class AddrFmtTestData
{
	public double value;
	public String result;

	public String addr;

	public int nzSign;
	public int nzLeadZeros;
	public int nzAbscissa;
	public int nzDecimal;
	public int nzTrailZeros;
	public int nzMantissa;

	public String zFormat;

	public AddrFmtTestData(
			double value,
			String result,

			String addr,

			int nzSign,
			int nzLeadZeros,
			int nzAbscissa,
			int nzDecimal,
			int nzTrailZeros,
			int nzMantissa,

			String zFormat )
	{
			this.value = value;
			this.result = result;

			this.addr = addr;

			this.nzSign = nzSign;
			this.nzLeadZeros = nzLeadZeros;
			this.nzAbscissa = nzAbscissa;
			this.nzDecimal = nzDecimal;
			this.nzTrailZeros = nzTrailZeros;
			this.nzMantissa = nzMantissa;

			this.zFormat = zFormat;
	}
}
