import { Link } from 'react-router-dom'
import { Sprout, Droplets, ChevronRight, Plus } from 'lucide-react'
import StatusPill from '@/components/status-pill/StatusPill.component'
import { listChannels, isOnline } from '@/utils/Devices'
import { cn } from '@/lib/utils'
import type { Device } from '@/redux/types/Device.type'
import PairDeviceDialogContainer from '@/containers/dashboard/plants/PairDeviceDialog.container'

interface PlantsProps {
	devices: Device[] | null
	lastAddedId: string | null
	pairOpen: boolean
	onPair: () => void
	onPairOpenChange: (open: boolean) => void
}

function Plants({ devices, lastAddedId, pairOpen, onPair, onPairOpenChange }: PlantsProps) {
	return (
		<>
			{devices === null ? (
				<SkeletonList />
			) : devices.length === 0 ? (
				<EmptyState onPair={onPair} />
			) : (
				<ul className="space-y-3">
					{devices.map(d => {
						const channels = listChannels(d.status)
						const moisture = channels.find(c => c.type === 'moisture')
						const relay = channels.find(c => c.type === 'relay')
						const online = isOnline(d.status?.lastSeen)
						const highlighted = d.id === lastAddedId

						return (
							<li
								key={d.id}
								className={cn(
									'transition-all duration-500',
									highlighted && 'animate-in fade-in slide-in-from-top-2'
								)}
							>
								<Link
									to={`/dashboard/plants/${d.id}`}
									className={cn(
										'block card-soft p-4 transition-all active:scale-[0.99]',
										highlighted && 'ring-2 ring-primary/60 shadow-pop'
									)}
								>
									<div className="grid grid-cols-[auto_minmax(0,1fr)_auto] items-center gap-3">
										<div
											className={cn(
												'grid h-12 w-12 shrink-0 place-items-center rounded-2xl',
												online
													? 'bg-primary/10 text-primary'
													: 'bg-muted text-muted-foreground'
											)}
										>
											<Sprout className="h-6 w-6" />
										</div>
										<div className="min-w-0">
											<p className="truncate text-base font-extrabold text-foreground">
												{d.nickname || 'L.E.E.F. Device'}
											</p>
											<p className="truncate font-mono text-[11px] uppercase tracking-wider text-muted-foreground">
												{d.id}
											</p>
										</div>
										<ChevronRight className="h-5 w-5 shrink-0 text-muted-foreground" />
									</div>
									<div className="mt-3 flex flex-wrap items-center gap-2">
										<StatusPill lastSeen={d.status?.lastSeen} />
										{moisture && (
											<span className="inline-flex items-center gap-1.5 rounded-full bg-sky/15 px-2.5 py-1 text-xs font-bold text-sky">
												<Droplets className="h-3.5 w-3.5" />
												{Math.round(moisture.data.reading)}%
											</span>
										)}
										{relay && (
											<span
												className={cn(
													'inline-flex items-center gap-1.5 rounded-full px-2.5 py-1 text-xs font-bold',
													relay.data.state === 'ON'
														? 'bg-primary/15 text-primary'
														: 'bg-muted text-muted-foreground'
												)}
											>
												Pump {relay.data.state}
											</span>
										)}
									</div>
								</Link>
							</li>
						)
					})}
				</ul>
			)}

			<PairDeviceDialogContainer open={pairOpen} onOpenChange={onPairOpenChange} />
		</>
	)
}

function SkeletonList() {
	return (
		<ul className="space-y-3">
			{[0, 1].map(i => (
				<li key={i} className="card-soft h-28 animate-pulse bg-muted/40" />
			))}
		</ul>
	)
}

function EmptyState({ onPair }: { onPair: () => void }) {
	return (
		<div className="mt-8 text-center">
			<div className="mx-auto grid h-20 w-20 place-items-center rounded-3xl bg-primary/10 text-primary">
				<Sprout className="h-10 w-10" />
			</div>
			<h2 className="mt-5 text-xl font-extrabold text-foreground">No plants here yet</h2>
			<p className="mx-auto mt-2 max-w-xs text-sm text-muted-foreground">
				Pair your first L.E.E.F. device to start seeing soil moisture and watering history.
			</p>
			<button
				type="button"
				onClick={onPair}
				className="mt-6 inline-flex items-center gap-2 rounded-full bg-primary px-5 py-3 text-sm font-bold text-primary-foreground shadow-soft transition-colors hover:bg-primary-dark"
			>
				<Plus className="h-4 w-4" />
				Pair a device
			</button>
		</div>
	)
}

export default Plants
