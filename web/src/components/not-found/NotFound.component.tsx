import { navigate } from '@/utils/Navigator'

function NotFound() {
	return (
		<div className="flex min-h-screen items-center justify-center bg-background px-6">
			<div className="flex flex-col content-center items-center justify-center max-w-sm text-center">
				<h1 className="text-6xl font-black text-primary">404</h1>
				<h2 className="mt-3 text-lg font-bold text-foreground">This page wandered off</h2>
				<p className="mt-2 text-sm text-muted-foreground">
					Let's get you back to your plants.
				</p>
				<button
					type="button"
					onClick={() => navigate('/dashboard')}
					className="mt-4 flex h-12 items-center justify-center rounded-full bg-primary px-6 text-sm font-bold text-primary-foreground shadow-soft transition-colors hover:bg-primary-dark"
				>
					Back to safety
				</button>
			</div>
		</div>
	)
}

export default NotFound
