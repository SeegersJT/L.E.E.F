import { LogOut, Moon, Sun, User as UserIcon } from 'lucide-react'
import type { AuthUser } from '@/redux/types/Authentication.type'

interface AccountProps {
	user: AuthUser
	dark: boolean
	busy: boolean
	onToggleTheme: () => void
	onLogout: () => void
}

function Account({ user, dark, busy, onToggleTheme, onLogout }: AccountProps) {
	const initial = (user.email || user.displayName || 'P').trim().charAt(0).toUpperCase()

	return (
		<>
			<div className="card-soft p-5">
				<div className="grid grid-cols-[auto_minmax(0,1fr)] items-center gap-4">
					<div className="grid h-14 w-14 shrink-0 place-items-center rounded-3xl bg-primary text-2xl font-black text-primary-foreground">
						{initial}
					</div>
					<div className="min-w-0">
						<p className="truncate text-base font-extrabold text-foreground">
							{user.displayName || 'Plant parent'}
						</p>
						<p className="truncate text-xs text-muted-foreground">{user.email}</p>
					</div>
				</div>
			</div>

			<ul className="card-soft mt-4 divide-y divide-border/70">
				<li>
					<button
						type="button"
						onClick={onToggleTheme}
						className="grid w-full grid-cols-[auto_minmax(0,1fr)_auto] items-center gap-3 px-4 py-4 text-left"
					>
						<span className="grid h-9 w-9 place-items-center rounded-xl bg-accent text-accent-foreground">
							{dark ? <Moon className="h-4 w-4" /> : <Sun className="h-4 w-4" />}
						</span>
						<span className="min-w-0">
							<span className="block text-sm font-bold text-foreground">
								Appearance
							</span>
							<span className="block text-xs text-muted-foreground">
								{dark ? 'Dark theme' : 'Light theme'}
							</span>
						</span>
						<span className="shrink-0 text-xs font-bold text-primary">Toggle</span>
					</button>
				</li>
				<li className="grid grid-cols-[auto_minmax(0,1fr)_auto] items-center gap-3 px-4 py-4">
					<span className="grid h-9 w-9 place-items-center rounded-xl bg-accent text-accent-foreground">
						<UserIcon className="h-4 w-4" />
					</span>
					<span className="min-w-0">
						<span className="block text-sm font-bold text-foreground">
							Signed in as
						</span>
						<span className="block truncate text-xs text-muted-foreground">
							{user.email}
						</span>
					</span>
				</li>
			</ul>

			<button
				type="button"
				onClick={onLogout}
				disabled={busy}
				className="mt-6 flex h-12 w-full items-center justify-center gap-2 rounded-full border border-destructive/40 bg-card text-sm font-bold text-destructive transition-colors hover:bg-destructive/5 disabled:opacity-60"
			>
				<LogOut className="h-4 w-4" /> {busy ? 'Signing out…' : 'Sign out'}
			</button>

			<p className="mt-6 text-center text-[11px] text-muted-foreground">
				L.E.E.F. companion • v0.0.0.1
			</p>
		</>
	)
}

export default Account
