
package Weng.System;

/**
 * All system Code Generators and Macros <b>must</b> be
 * implemented as either a WengMacro or a WengFrame.
 */
public interface WengMacro
{
    void main();
}

/* 2006.12.10 (PE) -- We could declare WengMacro as follows, but doing so
 * would also require us to update all of our (and our clients) macros such
 * that classes are declared as
 *
 *   public class MyMacro extends WengMacro
 *
 * instead of
 *
 *   public class MyMacro implements WengMacro
 *
 * Instead, we've opted to let WengVmEnv handle the differences.
 *
public abstract class WengMacro
{
    public void main()
    {
    }
}
*/
