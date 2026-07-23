import { Outlet } from 'react-router-dom'
import type { ReactNode } from 'react'
import { Home, BookOpen, User } from 'lucide-react'
import { cn } from '@/lib/utils'

export interface DashboardTab {
	path: string
	label: string
	title: string
	icon: typeof Home
}

export const DASHBOARD_TABS: DashboardTab[] = [
	{ path: 'plants', label: 'Plants', title: 'My Plants', icon: Home },
	{ path: 'setup', label: 'Setup', title: 'Set up device', icon: BookOpen },
	{ path: 'account', label: 'Account', title: 'Account', icon: User },
]

export interface DashboardOutletContext {
	setHeaderRight: (node: ReactNode) => void
	setTitle: (title: string | null) => void
}

interface DashboardProps {
	title: string
	headerRight: ReactNode
	activeTabPath: string
	onTabChange: (path: string) => void
	outletContext: DashboardOutletContext
}

function Dashboard({
	title,
	headerRight,
	activeTabPath,
	onTabChange,
	outletContext,
}: DashboardProps) {
	return (
		<div className="flex min-h-screen flex-col bg-background">
			<header className="flex items-center justify-between border-b border-border px-6 py-5">
				<h1 className="truncate text-2xl font-extrabold text-foreground">{title}</h1>
				{headerRight}
			</header>

			<main className="flex-1 overflow-y-auto px-6 py-5 pb-24">
				<Outlet context={outletContext} />
			</main>

			<nav className="fixed inset-x-0 bottom-0 flex border-t border-border bg-background px-2 pb-[env(safe-area-inset-bottom)]">
				{DASHBOARD_TABS.map(tab => {
					const Icon = tab.icon
					const active = tab.path === activeTabPath
					return (
						<button
							key={tab.path}
							type="button"
							onClick={() => onTabChange(tab.path)}
							className={cn(
								'group flex flex-1 flex-col items-center gap-1 py-3 text-xs font-bold transition-colors',
								active
									? 'text-primary'
									: 'text-muted-foreground hover:text-foreground'
							)}
						>
							<span
								className={cn(
									'grid h-8 w-8 place-items-center rounded-xl transition-colors',
									active ? 'bg-primary/10 text-primary' : 'group-hover:bg-muted'
								)}
							>
								<Icon className="h-5 w-5" />
							</span>
							{tab.label}
						</button>
					)
				})}
			</nav>
		</div>
	)
}

export default Dashboard
