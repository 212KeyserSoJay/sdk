/**
 * @file mega/win32/drivenotifywin.h
 * @brief Mega SDK various utilities and helper classes
 *
 * (c) 2013-2020 by Mega Limited, Auckland, New Zealand
 *
 * This file is part of the MEGA SDK - Client Access Engine.
 *
 * Applications using the MEGA API must present a valid application key
 * and comply with the the rules set forth in the Terms of Service.
 *
 * The MEGA SDK is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * @copyright Simplified (2-clause) BSD License.
 *
 * You should have received a copy of the license along with this
 * program.
 */

#pragma once

#ifdef USE_DRIVE_NOTIFICATIONS

// Include "mega/drivenotify.h" where needed.
// This header cannot be used by itself.

#include <map>
#include <vector>
#include <thread>
#include <atomic>

struct IWbemServices;
struct IWbemClassObject;


namespace mega {

    // Windows: Platform specific definition
    //
    // Uses WMI. Use 'wbemtest' tool to run WQL queries, for testing and comparison.
    class DriveNotifyWin : public DriveNotify
    {
    public:
        DriveNotifyWin() : mStop(false) {}
        ~DriveNotifyWin() override { stop(); }

    protected:
        bool startNotifier() override;
        void stopNotifier() override;

    private:
        bool doInThread();

        std::atomic_bool mStop;
        std::thread mEventSinkThread;

        enum EventType
        {
            UNKNOWN_EVENT,
            DRIVE_CONNECTED_EVENT,
            DRIVE_DISCONNECTED_EVENT
        };
    };



    // Windows: Platform specific definition
    //
    class UniqueDriveIdWin : public UniqueDriveId
    {
    protected:
        std::map<int, std::string> getIds(const std::string& mountPoint) override;

    private:
        std::vector<std::wstring> getWqlValues(IWbemServices* pService, const std::wstring& query, const std::vector<std::wstring>& fields,
                               const std::vector<std::function<std::wstring(IWbemClassObject*, const std::wstring&)>>* convFuncs = nullptr);
        std::wstring convertUi32ToB16str(IWbemClassObject* queryObj, const std::wstring& field);
        std::wstring getIdFromPNPDevId(const std::wstring& pnpIdString);
    };



    // Class for listing drives available at any moment.
    // This class queries 'Win32_LogicalDisk' for drives available to the user, that are assigned a drive letter.
    // In the same way, for further information about the partition and physical drive, other providers can be queried:
    // - Win32_LogicalDiskToPartition
    // - Win32_DiskPartition
    // - Win32_DiskDriveToDiskPartition
    // - Win32_DiskDrive
    // - Win32_MappedLogicalDisk -- only for mapped drives
    //
    // Other non-WMI Volume Management functions available under MS Windows:
    // https://docs.microsoft.com/en-us/windows/win32/fileio/volume-management-functions
    // Nice article about possible Dynamic Disk Structures:
    // https://www.apriorit.com/dev-blog/345-dynamic-disk-structure-parser
    class VolumeQuery
    {
    public:
        // Query 'Win32_LogicalDisk' for all drives with a drive letter assigned.
        // Returns a map of {drive-letter (i.e. L"C:"), DriveInfo} pairs.
        std::map<std::wstring, DriveInfo> query();
    };

} // namespace

#endif // USE_DRIVE_NOTIFICATIONS
