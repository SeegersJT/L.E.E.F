import { Link } from 'react-router'

function NotFound() {
	return (
		<div className="flex min-h-screen items-center justify-center bg-background px-6">
			<div className="max-w-sm text-center">
				<h1 className="text-6xl font-black text-primary">404</h1>
				<h2 className="mt-3 text-lg font-bold text-foreground">This page wandered off</h2>
				<p className="mt-2 text-sm text-muted-foreground">
					Let's get you back to your plants.
				</p>
				<Link
					to="/"
					className="mt-6 inline-flex items-center justify-center rounded-full bg-primary px-5 py-2.5 text-sm font-bold text-primary-foreground shadow-soft transition-colors hover:bg-primary-dark"
				>
					Go home
				</Link>
			</div>
		</div>
	)
}

export default NotFound
