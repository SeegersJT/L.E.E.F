function AuthenticateContainer() {
	const { signIn, signUp, signInWithGoogle, resetPassword, mockMode } = useAuth()
	const [mode, setMode] = useState<AuthMode>('signin')
	const [email, setEmail] = useState(mockMode ? 'plantparent@example.com' : '')
	const [password, setPassword] = useState(mockMode ? 'demo-password' : '')
	const [busy, setBusy] = useState(false)

	const handleSubmit = async (e: FormEvent) => {
		e.preventDefault()
		setBusy(true)
		try {
			if (mode === 'signin') {
				await signIn(email, password)
			} else if (mode === 'signup') {
				await signUp(email, password)
				toast.success('Welcome! Your account is ready.')
			} else {
				await resetPassword(email)
				toast.success('Password reset link sent — check your inbox.')
				setMode('signin')
			}
		} catch (err) {
			const msg = err instanceof Error ? err.message : 'Something went wrong'
			toast.error(msg)
		} finally {
			setBusy(false)
		}
	}

	const handleGoogleSignIn = async () => {
		setBusy(true)
		try {
			await signInWithGoogle()
		} catch (err) {
			const msg = err instanceof Error ? err.message : 'Google sign-in failed'
			toast.error(msg)
		} finally {
			setBusy(false)
		}
	}

	return (
		<AuthScreenView
			mode={mode}
			email={email}
			password={password}
			busy={busy}
			mockMode={mockMode}
			onModeChange={setMode}
			onEmailChange={setEmail}
			onPasswordChange={setPassword}
			onSubmit={handleSubmit}
			onGoogleSignIn={handleGoogleSignIn}
		/>
	)
}

export default AuthenticateContainer
