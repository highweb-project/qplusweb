/*
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "config.h"
#include "Clipboard.h"
#include "Pasteboard.h"
#include "DataObjectGtk.h"

#include "DataTransferItemListGtk.h"
#include <wtf/RefPtr.h>

namespace WebCore {
#if MODIFY(ENGINE)
#if ENABLE(DATA_TRANSFER_ITEMS)
PassRefPtr<DataTransferItemList> Clipboard::items() 
{  
    PassRefPtr<DataObjectGtk> dataObject = m_pasteboard->dataObject();
    return dataObject.get()->itemList();
}
#endif
#endif

}
