import { useState, type FormEvent } from 'react'
import { useAppDispatch } from '@/hooks/useAppDispatch'
import { useAppSelector } from '@/hooks/useAppSelector'
import {
	requestFirebaseEmailLogin,
	requestFirebaseRegister,
	requestFirebaseGoogleLogin,
	requestPasswordReset,
} from '@/redux/actions/Authentication.action'
import Authenticate, { type AuthMode } from '@/components/authenticate/Authenticate.component'

function AuthenticateContainer() {
	const dispatch = useAppDispatch()
	const busy = useAppSelector(state => state.auth.loading)

	const [mode, setMode] = useState<AuthMode>('signin')
	const [fullName, setFullName] = useState('')
	const [email, setEmail] = useState('')
	const [password, setPassword] = useState('')

	const onSubmit = (e: FormEvent) => {
		e.preventDefault()

		if (mode === 'signin') {
			dispatch(requestFirebaseEmailLogin({ email, password }))
		} else if (mode === 'signup') {
			dispatch(requestFirebaseRegister({ fullName, email, password }))
		} else {
			dispatch(
				requestPasswordReset({
					email,
					onSuccess: () => setMode('signin'),
				})
			)
		}
	}

	const onGoogleSignIn = () => {
		dispatch(requestFirebaseGoogleLogin())
	}

	return (
		<Authenticate
			mode={mode}
			fullName={fullName}
			email={email}
			password={password}
			busy={busy}
			onModeChange={setMode}
			onFullNameChange={setFullName}
			onEmailChange={setEmail}
			onPasswordChange={setPassword}
			onSubmit={onSubmit}
			onGoogleSignIn={onGoogleSignIn}
		/>
	)
}

export default AuthenticateContainer
