import type { FormEvent } from 'react'
import { Sprout, Leaf } from 'lucide-react'
import type React from 'react'

export type AuthMode = 'signin' | 'signup' | 'forgot'

interface AuthenticateProps {
	mode: AuthMode
	fullName: string
	email: string
	password: string
	busy: boolean
	onModeChange: (mode: AuthMode) => void
	onFullNameChange: (value: string) => void
	onEmailChange: (value: string) => void
	onPasswordChange: (value: string) => void
	onSubmit: (e: FormEvent) => void
	onGoogleSignIn: () => void
}

function Authenticate({
	mode,
	fullName,
	email,
	password,
	busy,
	onModeChange,
	onFullNameChange,
	onEmailChange,
	onPasswordChange,
	onSubmit,
	onGoogleSignIn,
}: AuthenticateProps) {
	return (
		<div className="min-h-screen bg-background px-6 py-10">
			<div className="mx-auto max-w-sm">
				<div className="flex flex-col items-center text-center">
					<div className="grid h-16 w-16 place-items-center rounded-3xl bg-primary text-primary-foreground shadow-soft">
						<Leaf className="h-8 w-8" />
					</div>
					<h1 className="mt-5 text-3xl font-black tracking-tight text-foreground">
						L.E.E.F.
					</h1>
					<p className="mt-1 text-sm text-muted-foreground">
						Little tender care for your leafy friends.
					</p>
				</div>

				<div className="mt-6 flex rounded-full bg-muted p-1">
					<TabBtn active={mode !== 'signup'} onClick={() => onModeChange('signin')}>
						Sign in
					</TabBtn>
					<TabBtn active={mode === 'signup'} onClick={() => onModeChange('signup')}>
						Create account
					</TabBtn>
				</div>

				<form onSubmit={onSubmit} className="mt-5 space-y-3">
					{mode === 'signup' && (
						<Field
							label="Full name"
							type="text"
							autoComplete="name"
							value={fullName}
							onChange={onFullNameChange}
							required
						/>
					)}
					<Field
						label="Email"
						type="email"
						autoComplete="email"
						value={email}
						onChange={onEmailChange}
						required
					/>
					{mode !== 'forgot' && (
						<Field
							label="Password"
							type="password"
							autoComplete={mode === 'signup' ? 'new-password' : 'current-password'}
							value={password}
							onChange={onPasswordChange}
							required
							minLength={6}
						/>
					)}

					<button
						type="submit"
						disabled={busy}
						className="mt-2 flex h-12 w-full items-center justify-center gap-2 rounded-full bg-primary text-sm font-bold text-primary-foreground shadow-soft transition-colors hover:bg-primary-dark disabled:opacity-60"
					>
						{busy ? (
							<Sprout className="h-4 w-4 animate-pulse" />
						) : mode === 'signin' ? (
							'Sign in'
						) : mode === 'signup' ? (
							'Create account'
						) : (
							'Send reset link'
						)}
					</button>
				</form>

				{mode !== 'forgot' && (
					<>
						<div className="my-5 flex items-center gap-3 text-[11px] font-bold uppercase tracking-wider text-muted-foreground">
							<span className="h-px flex-1 bg-border" /> or{' '}
							<span className="h-px flex-1 bg-border" />
						</div>
						<button
							type="button"
							onClick={onGoogleSignIn}
							disabled={busy}
							className="flex h-12 w-full items-center justify-center gap-2 rounded-full border border-input bg-card text-sm font-bold text-foreground transition-colors hover:bg-accent disabled:opacity-60"
						>
							<GoogleG /> Continue with Google
						</button>
					</>
				)}

				<div className="mt-6 text-center text-xs text-muted-foreground">
					{mode === 'forgot' ? (
						<button
							className="font-semibold text-primary"
							onClick={() => onModeChange('signin')}
						>
							Back to sign in
						</button>
					) : (
						<button
							className="font-semibold text-primary"
							onClick={() => onModeChange('forgot')}
						>
							Forgot password?
						</button>
					)}
				</div>
			</div>
		</div>
	)
}

function TabBtn({
	active,
	onClick,
	children,
}: {
	active: boolean
	onClick: () => void
	children: React.ReactNode
}) {
	return (
		<button
			type="button"
			onClick={onClick}
			className={`h-10 flex-1 rounded-full text-sm font-bold transition-colors ${
				active ? 'bg-card text-foreground shadow-soft' : 'text-muted-foreground'
			}`}
		>
			{children}
		</button>
	)
}

function Field({
	label,
	value,
	onChange,
	...rest
}: {
	label: string
	value: string
	onChange: (v: string) => void
} & Omit<React.InputHTMLAttributes<HTMLInputElement>, 'value' | 'onChange'>) {
	return (
		<label className="block">
			<span className="mb-1.5 block text-xs font-bold uppercase tracking-wider text-muted-foreground">
				{label}
			</span>
			<input
				{...rest}
				value={value}
				onChange={e => onChange(e.target.value)}
				className="h-12 w-full rounded-2xl border border-input bg-card px-4 text-sm text-foreground outline-none transition-shadow placeholder:text-muted-foreground focus:ring-4 focus:ring-primary/20"
			/>
		</label>
	)
}

function GoogleG() {
	return (
		<svg viewBox="0 0 48 48" className="h-5 w-5" aria-hidden>
			<path
				fill="#EA4335"
				d="M24 9.5c3.5 0 6.6 1.2 9 3.5l6.7-6.7C35.7 2.5 30.2 0 24 0 14.6 0 6.5 5.4 2.6 13.2l7.9 6.1C12.4 13.4 17.7 9.5 24 9.5z"
			/>
			<path
				fill="#4285F4"
				d="M46.5 24.5c0-1.6-.2-3.1-.5-4.5H24v9h12.7c-.6 3-2.3 5.6-4.9 7.3l7.6 5.9c4.4-4.1 7.1-10.1 7.1-17.7z"
			/>
			<path
				fill="#FBBC05"
				d="M10.5 28.7c-.5-1.5-.8-3.1-.8-4.7s.3-3.2.8-4.7l-7.9-6.1C.9 16.5 0 20.1 0 24s.9 7.5 2.6 10.8l7.9-6.1z"
			/>
			<path
				fill="#34A853"
				d="M24 48c6.5 0 11.9-2.1 15.8-5.8l-7.6-5.9c-2.1 1.4-4.8 2.2-8.2 2.2-6.3 0-11.6-3.9-13.5-9.3l-7.9 6.1C6.5 42.6 14.6 48 24 48z"
			/>
		</svg>
	)
}

export default Authenticate
