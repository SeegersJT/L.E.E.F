import { BrowserRouter, Routes, Route, Navigate } from 'react-router-dom'
import ScrollToTop from '@/components/scroll-to-top/ScrollToTop.component'
import { NavigateInjector } from '@/components/navigate-injector/NavigateInjector.component'
import NotFoundContainer from '@/containers/not-found/NotFound.container'
import AuthenticateContainer from '@/containers/authenticate/Authenticate.container'

export const AppRouter = () => (
	<BrowserRouter>
		<Routes>
			<Route path={'/'} element={<Navigate to={'/login'} replace />} />

			<Route path={'/login'} element={<AuthenticateContainer />} />

			<Route path="*" element={<NotFoundContainer />} />
		</Routes>

		<ScrollToTop />
		<NavigateInjector />
	</BrowserRouter>
)
