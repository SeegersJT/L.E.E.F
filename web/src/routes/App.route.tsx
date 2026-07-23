import { BrowserRouter, Routes, Route, Navigate } from 'react-router-dom'
import ScrollToTop from '@/components/scroll-to-top/ScrollToTop.component'
import { NavigateInjector } from '@/components/navigate-injector/NavigateInjector.component'
import NotFoundContainer from '@/containers/not-found/NotFound.container'
import AuthenticateContainer from '@/containers/authenticate/Authenticate.container'
import RequireAuth from '@/components/require-auth/RequireAuth.component'
import DashboardContainer from '@/containers/dashboard/Dashboard.container'
import PlantsContainer from '@/containers/dashboard/plants/Plants.container'
import SetupContainer from '@/containers/dashboard/setup/Setup.container'
import AccountContainer from '@/containers/dashboard/account/Account.container'
import DeviceDetailContainer from '@/containers/device-detail/DeviceDetail.container'

export const AppRouter = () => (
	<BrowserRouter>
		<Routes>
			<Route path={'/'} element={<Navigate to={'/login'} replace />} />

			<Route path={'/login'} element={<AuthenticateContainer />} />

			<Route element={<RequireAuth />}>
				<Route path="/dashboard" element={<DashboardContainer />}>
					<Route index element={<Navigate to="plants" replace />} />

					<Route path={'plants'} element={<PlantsContainer />} />
					<Route path={'plants/:deviceId'} element={<DeviceDetailContainer />} />
					<Route path={'setup'} element={<SetupContainer />} />
					<Route path={'account'} element={<AccountContainer />} />
				</Route>
			</Route>

			<Route path="*" element={<NotFoundContainer />} />
		</Routes>

		<ScrollToTop />
		<NavigateInjector />
	</BrowserRouter>
)
