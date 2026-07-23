import { useMemo, useState, type ReactNode } from 'react'
import { useLocation, useNavigate } from 'react-router-dom'
import Dashboard, { DASHBOARD_TABS } from '@/components/dashboard/Dashboard.component'

function DashboardContainer() {
	const location = useLocation()
	const navigate = useNavigate()
	const [headerRight, setHeaderRight] = useState<ReactNode>(null)
	const [titleOverride, setTitleOverride] = useState<string | null>(null)

	const activeTab = useMemo(
		() =>
			DASHBOARD_TABS.find(tab => location.pathname.startsWith(`/dashboard/${tab.path}`)) ??
			DASHBOARD_TABS[0],
		[location.pathname]
	)

	return (
		<Dashboard
			title={titleOverride ?? activeTab.title}
			headerRight={headerRight}
			activeTabPath={activeTab.path}
			onTabChange={path => navigate(`/dashboard/${path}`)}
			outletContext={{ setHeaderRight, setTitle: setTitleOverride }}
		/>
	)
}

export default DashboardContainer
