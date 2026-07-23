import { useEffect, useState } from 'react'
import { useAppSelector } from '@/hooks/useAppSelector'
import { useAppDispatch } from '@/hooks/useAppDispatch'
import { requestLogout } from '@/redux/actions/Authentication.action'
import Account from '@/components/dashboard/account/Account.component'

function AccountContainer() {
	const dispatch = useAppDispatch()
	const user = useAppSelector(state => state.auth.user)
	const busy = useAppSelector(state => state.auth.loading)
	const [dark, setDark] = useState(false)

	useEffect(() => {
		const saved = window.localStorage.getItem('leef-theme')
		const isDark = saved === 'dark'
		setDark(isDark)
		document.documentElement.classList.toggle('dark', isDark)
	}, [])

	const onToggleTheme = () => {
		const next = !dark
		setDark(next)
		document.documentElement.classList.toggle('dark', next)
		window.localStorage.setItem('leef-theme', next ? 'dark' : 'light')
	}

	const onLogout = () => {
		dispatch(requestLogout())
	}

	if (!user) return null

	return (
		<Account
			user={user}
			dark={dark}
			busy={busy}
			onToggleTheme={onToggleTheme}
			onLogout={onLogout}
		/>
	)
}

export default AccountContainer
