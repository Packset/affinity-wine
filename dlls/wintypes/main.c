/*
 * Copyright 2022 Zhiyi Zhang for CodeWeavers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <stdarg.h>

#define COBJMACROS
#include "initguid.h"
#include "windef.h"
#include "winbase.h"
#include "winstring.h"
#include "wine/debug.h"
#include "objbase.h"

#include "fileapi.h"
#include "activation.h"
#include "rometadataresolution.h"

#define WIDL_using_Windows_Foundation_Metadata
#include "windows.foundation.metadata.h"

WINE_DEFAULT_DEBUG_CHANNEL(wintypes);

static const struct
{
    const WCHAR *name;
    unsigned int max_major;
}
present_contracts[] =
{
    { L"Windows.Foundation.UniversalApiContract", 10, },
};

static BOOLEAN is_api_contract_present( const HSTRING hname, unsigned int version )
{
    const WCHAR *name = WindowsGetStringRawBuffer( hname, NULL );
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(present_contracts); ++i)
        if (!wcsicmp( name, present_contracts[i].name )) return version <= present_contracts[i].max_major;

    return FALSE;
}

struct wintypes
{
    IActivationFactory IActivationFactory_iface;
    IApiInformationStatics IApiInformationStatics_iface;
    LONG ref;
};

static inline struct wintypes *impl_from_IActivationFactory(IActivationFactory *iface)
{
    return CONTAINING_RECORD(iface, struct wintypes, IActivationFactory_iface);
}

static inline struct wintypes *impl_from_IApiInformationStatics(IApiInformationStatics *iface)
{
    return CONTAINING_RECORD(iface, struct wintypes, IApiInformationStatics_iface);
}

static HRESULT STDMETHODCALLTYPE wintypes_QueryInterface(IActivationFactory *iface, REFIID iid,
        void **out)
{
    struct wintypes *impl = impl_from_IActivationFactory(iface);

    TRACE("iface %p, iid %s, out %p stub!\n", iface, debugstr_guid(iid), out);

    if (IsEqualGUID(iid, &IID_IUnknown)
            || IsEqualGUID(iid, &IID_IInspectable)
            || IsEqualGUID(iid, &IID_IAgileObject)
            || IsEqualGUID(iid, &IID_IActivationFactory))
    {
        IUnknown_AddRef(iface);
        *out = iface;
        return S_OK;
    }

    if (IsEqualGUID(iid, &IID_IApiInformationStatics))
    {
        IUnknown_AddRef(iface);
        *out = &impl->IApiInformationStatics_iface;
        return S_OK;
    }

    FIXME("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE wintypes_AddRef(IActivationFactory *iface)
{
    struct wintypes *impl = impl_from_IActivationFactory(iface);
    ULONG ref = InterlockedIncrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE wintypes_Release(IActivationFactory *iface)
{
    struct wintypes *impl = impl_from_IActivationFactory(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static HRESULT STDMETHODCALLTYPE wintypes_GetIids(IActivationFactory *iface, ULONG *iid_count,
        IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE wintypes_GetRuntimeClassName(IActivationFactory *iface,
        HSTRING *class_name)
{
    FIXME("iface %p, class_name %p stub!\n", iface, class_name);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE wintypes_GetTrustLevel(IActivationFactory *iface,
        TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE wintypes_ActivateInstance(IActivationFactory *iface,
        IInspectable **instance)
{
    FIXME("iface %p, instance %p stub!\n", iface, instance);
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl activation_factory_vtbl =
{
    wintypes_QueryInterface,
    wintypes_AddRef,
    wintypes_Release,
    /* IInspectable methods */
    wintypes_GetIids,
    wintypes_GetRuntimeClassName,
    wintypes_GetTrustLevel,
    /* IActivationFactory methods */
    wintypes_ActivateInstance,
};

static HRESULT STDMETHODCALLTYPE api_information_statics_QueryInterface(
        IApiInformationStatics *iface, REFIID iid, void **out)
{
    struct wintypes *impl = impl_from_IApiInformationStatics(iface);
    return wintypes_QueryInterface(&impl->IActivationFactory_iface, iid, out);
}

static ULONG STDMETHODCALLTYPE api_information_statics_AddRef(
        IApiInformationStatics *iface)
{
    struct wintypes *impl = impl_from_IApiInformationStatics(iface);
    return wintypes_AddRef(&impl->IActivationFactory_iface);
}

static ULONG STDMETHODCALLTYPE api_information_statics_Release(
        IApiInformationStatics *iface)
{
    struct wintypes *impl = impl_from_IApiInformationStatics(iface);
    return wintypes_Release(&impl->IActivationFactory_iface);
}

static HRESULT STDMETHODCALLTYPE api_information_statics_GetIids(
        IApiInformationStatics *iface, ULONG *iid_count, IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE api_information_statics_GetRuntimeClassName(
        IApiInformationStatics *iface, HSTRING *class_name)
{
    FIXME("iface %p, class_name %p stub!\n", iface, class_name);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE api_information_statics_GetTrustLevel(
        IApiInformationStatics *iface, TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE api_information_statics_IsTypePresent(
        IApiInformationStatics *iface, HSTRING type_name, BOOLEAN *value)
{
    FIXME("iface %p, type_name %s, value %p stub!\n", iface, debugstr_hstring(type_name), value);

    if (!type_name)
        return E_INVALIDARG;

    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE api_information_statics_IsMethodPresent(
        IApiInformationStatics *iface, HSTRING type_name, HSTRING method_name, BOOLEAN *value)
{
    FIXME("iface %p, type_name %s, method_name %s, value %p stub!\n", iface,
            debugstr_hstring(type_name), debugstr_hstring(method_name), value);

    if (!type_name)
        return E_INVALIDARG;

    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE api_information_statics_IsMethodPresentWithArity(
        IApiInformationStatics *iface, HSTRING type_name, HSTRING method_name,
        UINT32 input_parameter_count, BOOLEAN *value)
{
    FIXME("iface %p, type_name %s, method_name %s, input_parameter_count %u, value %p stub!\n",
            iface, debugstr_hstring(type_name), debugstr_hstring(method_name),
            input_parameter_count, value);

    if (!type_name)
        return E_INVALIDARG;

    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE api_information_statics_IsEventPresent(
        IApiInformationStatics *iface, HSTRING type_name, HSTRING event_name, BOOLEAN *value)
{
    FIXME("iface %p, type_name %s, event_name %s, value %p stub!\n", iface,
            debugstr_hstring(type_name), debugstr_hstring(event_name), value);

    if (!type_name)
        return E_INVALIDARG;

    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE api_information_statics_IsPropertyPresent(
        IApiInformationStatics *iface, HSTRING type_name, HSTRING property_name, BOOLEAN *value)
{
    FIXME("iface %p, type_name %s, property_name %s, value %p stub!\n", iface,
            debugstr_hstring(type_name), debugstr_hstring(property_name), value);

    if (!type_name)
        return E_INVALIDARG;

    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE api_information_statics_IsReadOnlyPropertyPresent(
        IApiInformationStatics *iface, HSTRING type_name, HSTRING property_name,
        BOOLEAN *value)
{
    FIXME("iface %p, type_name %s, property_name %s, value %p stub!\n", iface,
            debugstr_hstring(type_name), debugstr_hstring(property_name), value);

    if (!type_name)
        return E_INVALIDARG;

    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE api_information_statics_IsWriteablePropertyPresent(
        IApiInformationStatics *iface, HSTRING type_name, HSTRING property_name, BOOLEAN *value)
{
    FIXME("iface %p, type_name %s, property_name %s, value %p stub!\n", iface,
            debugstr_hstring(type_name), debugstr_hstring(property_name), value);

    if (!type_name)
        return E_INVALIDARG;

    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE api_information_statics_IsEnumNamedValuePresent(
        IApiInformationStatics *iface, HSTRING enum_type_name, HSTRING value_name, BOOLEAN *value)
{
    FIXME("iface %p, enum_type_name %s, value_name %s, value %p stub!\n", iface,
            debugstr_hstring(enum_type_name), debugstr_hstring(value_name), value);

    if (!enum_type_name)
        return E_INVALIDARG;

    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE api_information_statics_IsApiContractPresentByMajor(
        IApiInformationStatics *iface, HSTRING contract_name, UINT16 major_version, BOOLEAN *value)
{
    FIXME("iface %p, contract_name %s, major_version %u, value %p semi-stub.\n", iface,
            debugstr_hstring(contract_name), major_version, value);

    if (!contract_name)
        return E_INVALIDARG;

    *value = is_api_contract_present( contract_name, major_version );
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE api_information_statics_IsApiContractPresentByMajorAndMinor(
        IApiInformationStatics *iface, HSTRING contract_name, UINT16 major_version,
        UINT16 minor_version, BOOLEAN *value)
{
    FIXME("iface %p, contract_name %s, major_version %u, minor_version %u, value %p stub!\n", iface,
            debugstr_hstring(contract_name), major_version, minor_version, value);

    if (!contract_name)
        return E_INVALIDARG;

    return E_NOTIMPL;
}

static const struct IApiInformationStaticsVtbl api_information_statics_vtbl =
{
    api_information_statics_QueryInterface,
    api_information_statics_AddRef,
    api_information_statics_Release,
    /* IInspectable methods */
    api_information_statics_GetIids,
    api_information_statics_GetRuntimeClassName,
    api_information_statics_GetTrustLevel,
    /* IApiInformationStatics methods */
    api_information_statics_IsTypePresent,
    api_information_statics_IsMethodPresent,
    api_information_statics_IsMethodPresentWithArity,
    api_information_statics_IsEventPresent,
    api_information_statics_IsPropertyPresent,
    api_information_statics_IsReadOnlyPropertyPresent,
    api_information_statics_IsWriteablePropertyPresent,
    api_information_statics_IsEnumNamedValuePresent,
    api_information_statics_IsApiContractPresentByMajor,
    api_information_statics_IsApiContractPresentByMajorAndMinor
};

static struct wintypes wintypes =
{
    {&activation_factory_vtbl},
    {&api_information_statics_vtbl},
    1
};

HRESULT WINAPI DllGetClassObject(REFCLSID clsid, REFIID riid, void **out)
{
    FIXME("clsid %s, riid %s, out %p stub!\n", debugstr_guid(clsid), debugstr_guid(riid), out);
    return CLASS_E_CLASSNOTAVAILABLE;
}

HRESULT WINAPI DllGetActivationFactory(HSTRING classid, IActivationFactory **factory)
{
    TRACE("classid %s, factory %p.\n", debugstr_hstring(classid), factory);
    *factory = &wintypes.IActivationFactory_iface;
    IUnknown_AddRef(*factory);
    return S_OK;
}

HRESULT WINAPI RoIsApiContractMajorVersionPresent(const WCHAR *name, UINT16 major, BOOL *result)
{
    FIXME("name %s, major %u, result %p\n", debugstr_w(name), major, result);
    *result = FALSE;
    return S_OK;
}

HRESULT WINAPI RoResolveNamespace(HSTRING name, HSTRING windowsMetaDataDir,
                                  DWORD packageGraphDirsCount, const HSTRING *packageGraphDirs,
                                  DWORD *metaDataFilePathsCount, HSTRING **metaDataFilePaths,
                                  DWORD *subNamespacesCount, HSTRING **subNamespaces)
{
    const WCHAR *_name = WindowsGetStringRawBuffer(name, NULL);
    HRESULT err = S_OK;
    WCHAR winMdDir[MAX_PATH+1];
    WCHAR glob[MAX_PATH+10];
    UINT32 winMdDir_len;
    DWORD resCount = 0;
    UINT16 resSize = 4;
    HSTRING *res = calloc(resSize, sizeof(HRESULT));
    HSTRING *oldres;
    WIN32_FIND_DATAW f_data;
    HANDLE f_handle;
    WCHAR *basename;
    WCHAR *fullname;
    UINT32 i;

    FIXME("name %s, windowsMetaDataDir %s, metaDataFilePaths %p, subNamespaces %p stub!\n",
            debugstr_hstring(name), debugstr_hstring(windowsMetaDataDir),
            metaDataFilePaths, subNamespaces);

    if (!metaDataFilePaths && !subNamespaces)
        return E_INVALIDARG;

    if (subNamespaces) {
        FIXME("subNamespaces not implemented\n");
        return RO_E_METADATA_NAME_NOT_FOUND;
    }

    memset(winMdDir, 0, (MAX_PATH+1)*sizeof(WCHAR));
    memset(glob, 0, (MAX_PATH+1)*sizeof(WCHAR));

    if (!windowsMetaDataDir) {
        winMdDir_len = GetWindowsDirectoryW(winMdDir, MAX_PATH);
        wcscat(winMdDir, L"\\system32\\WinMetadata");
        winMdDir_len += 22;
    } else {
        winMdDir_len = WindowsGetStringLen(windowsMetaDataDir);
        memcpy(winMdDir, WindowsGetStringRawBuffer(windowsMetaDataDir, NULL), sizeof(WCHAR)*winMdDir_len);
    }

    memcpy(glob, winMdDir, wcslen(winMdDir)*sizeof(WCHAR));
    wcscat(glob, L"\\*.winmd");

    f_handle = FindFirstFileW(glob, &f_data);
    if (f_handle == INVALID_HANDLE_VALUE) {
        FIXME("NO .WINMD FILES.\n");
        return RO_E_METADATA_NAME_NOT_FOUND;
    }
    if (res == NULL)
        return E_OUTOFMEMORY;
    basename = calloc(MAX_PATH+1, sizeof(WCHAR));
    if (basename == NULL)
        return E_OUTOFMEMORY;
    fullname = calloc(MAX_PATH+wcslen(winMdDir)+1, sizeof(WCHAR));
    if (fullname == NULL)
        return E_OUTOFMEMORY;
    while (err == S_OK) {
        memset(basename, 0, (MAX_PATH+1)*sizeof(WCHAR));
        memset(fullname, 0, (MAX_PATH+wcslen(winMdDir)+1)*sizeof(WCHAR));
        lstrcpynW(basename, f_data.cFileName, wcslen(f_data.cFileName) - 5);
        wcscpy(fullname, winMdDir);
        if (
            (wcslen(_name) >= wcslen(basename) && wcsstr(_name, basename) == _name)
            || (wcslen(basename) > wcslen(_name) && wcsstr(basename, _name) == basename)
        ) {
            resCount ++;
            if (resCount > resSize) {
                oldres = res;
                res = realloc(res, sizeof(HSTRING)*resSize*2);
                if (res == NULL) {
                    err = E_OUTOFMEMORY;
                    res = oldres;
                    break;
                }
            }

            wcscat(fullname, L"\\");
            wcscat(fullname, f_data.cFileName);
            err = WindowsCreateString(fullname, wcslen(fullname), &res[resCount-1]);
            FIXME("RESOLVED: %s\n", wine_dbgstr_hstring(res[resCount-1]));
        }
        if (!FindNextFileW(f_handle, &f_data))
            break;
    }
    FindClose(f_handle);
    free(basename);
    free(fullname);
    if (!err) {
        if (resCount > 0) {
            *metaDataFilePaths = res;
            *metaDataFilePathsCount = resCount;
        } else {
            err = RO_E_METADATA_NAME_NOT_FOUND;
        }
    } else {
        for (i=0; i<resCount-1; i++) {
            WindowsDeleteString(res[i]);
        }
        free(res);
    }

    return err;
}
