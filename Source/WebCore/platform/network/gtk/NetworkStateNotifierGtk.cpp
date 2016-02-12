
#include "config.h"
#include "NetworkStateNotifier.h"
#include <gio/gio.h>
#include <glib-object.h>
#include <glib/gi18n-lib.h>

namespace WebCore
{

void networkChangedCallback(GNetworkMonitor* monitor, gboolean available, NetworkStateNotifier* notifier)
{
	if(!notifier)
		return; 

	notifier->setIsOnLine(available);
}

void NetworkStateNotifier::setIsOnLine(bool available)
{
	if(m_isOnLine == available)
		return;
	
	updateState();
}

NetworkStateNotifier::~NetworkStateNotifier()
{	
	m_monitor = NULL;
}

void NetworkStateNotifier::updateState()
{	
	if(!m_monitor)
		return;

 	m_isOnLine = g_network_monitor_get_network_available(m_monitor); 	
 	notifyNetworkStateChange();
}

NetworkStateNotifier::NetworkStateNotifier()
    : m_monitor(NULL)
    , m_isOnLine(false)    
{
    initialize();
}

void NetworkStateNotifier::initialize()
{
	m_monitor = g_network_monitor_get_default();
	g_assert(G_IS_NETWORK_MONITOR(m_monitor));
	g_signal_connect(m_monitor, "network_changed", G_CALLBACK(networkChangedCallback), this);
	updateState();
}

} // namespace WebCore