﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:4.0.30319.42000
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace Updater.Properties {
    using System;
    
    
    /// <summary>
    ///   A strongly-typed resource class, for looking up localized strings, etc.
    /// </summary>
    // This class was auto-generated by the StronglyTypedResourceBuilder
    // class via a tool like ResGen or Visual Studio.
    // To add or remove a member, edit your .ResX file then rerun ResGen
    // with the /str option, or rebuild your VS project.
    [global::System.CodeDom.Compiler.GeneratedCodeAttribute("System.Resources.Tools.StronglyTypedResourceBuilder", "16.0.0.0")]
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute()]
    [global::System.Runtime.CompilerServices.CompilerGeneratedAttribute()]
    internal class UpdateResource {
        
        private static global::System.Resources.ResourceManager resourceMan;
        
        private static global::System.Globalization.CultureInfo resourceCulture;
        
        [global::System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1811:AvoidUncalledPrivateCode")]
        internal UpdateResource() {
        }
        
        /// <summary>
        ///   Returns the cached ResourceManager instance used by this class.
        /// </summary>
        [global::System.ComponentModel.EditorBrowsableAttribute(global::System.ComponentModel.EditorBrowsableState.Advanced)]
        internal static global::System.Resources.ResourceManager ResourceManager {
            get {
                if (object.ReferenceEquals(resourceMan, null)) {
                    global::System.Resources.ResourceManager temp = new global::System.Resources.ResourceManager("Updater.Properties.UpdateResource", typeof(UpdateResource).Assembly);
                    resourceMan = temp;
                }
                return resourceMan;
            }
        }
        
        /// <summary>
        ///   Overrides the current thread's CurrentUICulture property for all
        ///   resource lookups using this strongly typed resource class.
        /// </summary>
        [global::System.ComponentModel.EditorBrowsableAttribute(global::System.ComponentModel.EditorBrowsableState.Advanced)]
        internal static global::System.Globalization.CultureInfo Culture {
            get {
                return resourceCulture;
            }
            set {
                resourceCulture = value;
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Updates are being executed..
        /// </summary>
        internal static string executeUpdate {
            get {
                return ResourceManager.GetString("executeUpdate", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Fubble is starting....
        /// </summary>
        internal static string fubbleStart {
            get {
                return ResourceManager.GetString("fubbleStart", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Error starting fubble: {0}.
        /// </summary>
        internal static string fubbleStartError {
            get {
                return ResourceManager.GetString("fubbleStartError", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Less Information.
        /// </summary>
        internal static string lessInformation {
            get {
                return ResourceManager.GetString("lessInformation", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to More Information.
        /// </summary>
        internal static string moreInformation {
            get {
                return ResourceManager.GetString("moreInformation", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to There was an error during the update..
        /// </summary>
        internal static string updateError {
            get {
                return ResourceManager.GetString("updateError", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Update successful..
        /// </summary>
        internal static string updateSuccess {
            get {
                return ResourceManager.GetString("updateSuccess", resourceCulture);
            }
        }
    }
}