import { Navigate, Outlet } from 'react-router-dom'
import { Sprout } from 'lucide-react'
import { useAppSelector } from '@/hooks/useAppSelector'

function RequireAuth() {
	const { user, initialized } = useAppSelector(state => state.auth)

	if (!initialized) {
		return (
			<div className="flex min-h-screen items-center justify-center bg-background">
				<Sprout className="h-6 w-6 animate-pulse text-primary" />
			</div>
		)
	}

	if (!user) {
		return <Navigate to="/login" replace />
	}

	return <Outlet />
}

export default RequireAuth
