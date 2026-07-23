import { Link } from 'react-router-dom'
import { useMemo, useState } from 'react'
import { ArrowLeft, Droplets, Power, Wifi, Info, ChevronDown, Sprout, Trash2 } from 'lucide-react'
import {
	ResponsiveContainer,
	LineChart,
	Line,
	XAxis,
	YAxis,
	Tooltip,
	CartesianGrid,
	ReferenceDot,
} from 'recharts'
import { format, formatDistanceToNowStrict } from 'date-fns'
import { cn } from '@/lib/utils'
import MoistureGauge from '@/components/moisture-gauge/MoistureGauge.component'
import type { Device, MoistureChannelStatus, RelayChannelStatus } from '@/redux/types/Device.type'
import type { MoistureHistoryPoint, RelayHistoryPoint } from '@/redux/types/DeviceHistory.type'
import RemoveDeviceDialogContainer from '@/containers/device-removal-dialog/DeviceRemovalDialog.container'

export type Range = '24h' | '7d' | '30d'

export const RANGE_MS: Record<Range, number> = {
	'24h': 24 * 3600_000,
	'7d': 7 * 24 * 3600_000,
	'30d': 30 * 24 * 3600_000,
}

const RANGES: Range[] = ['24h', '7d', '30d']

interface DeviceDetailProps {
	loading: boolean
	device: Device | null
	moisture: MoistureChannelStatus | undefined
	relay: RelayChannelStatus | undefined
	sending: boolean
	watering: boolean
	onWaterNow: () => void
	range: Range
	onRangeChange: (range: Range) => void
	historyLoading: boolean
	historyError: boolean
	moistureHistory: MoistureHistoryPoint[]
	relayHistory: RelayHistoryPoint[]
	removeOpen: boolean
	onRequestRemove: () => void
	onRemoveOpenChange: (open: boolean) => void
}

function DeviceDetail({
	loading,
	device,
	moisture,
	relay,
	sending,
	watering,
	onWaterNow,
	range,
	onRangeChange,
	historyLoading,
	historyError,
	moistureHistory,
	relayHistory,
	removeOpen,
	onRequestRemove,
	onRemoveOpenChange,
}: DeviceDetailProps) {
	if (loading) return <LoadingShell />
	if (!device) return <NotFoundShell />

	return (
		<>
			<BackLink />

			<div className="mt-2 mb-5">
				<p className="font-mono text-[11px] uppercase tracking-widest text-muted-foreground">
					{device.id}
				</p>
				<p className="mt-0.5 text-xs text-muted-foreground">
					Last seen{' '}
					{device.status?.lastSeen
						? formatDistanceToNowStrict(new Date(device.status.lastSeen), {
								addSuffix: true,
							})
						: 'never'}
				</p>
			</div>

			<div className="grid gap-3">
				{moisture && <MoistureCard data={moisture} />}
				{relay && (
					<RelayCard
						data={relay}
						sending={sending}
						watering={watering}
						onWaterNow={onWaterNow}
					/>
				)}
			</div>

			{moisture && (
				<HistorySection
					range={range}
					onRangeChange={onRangeChange}
					loading={historyLoading}
					error={historyError}
					moistureHistory={moistureHistory}
					relayHistory={relayHistory}
				/>
			)}

			<DeviceInfo status={device.status} onRemove={onRequestRemove} />

			<RemoveDeviceDialogContainer
				open={removeOpen}
				onOpenChange={onRemoveOpenChange}
				deviceId={device.id}
				nickname={device.nickname || 'this device'}
			/>
		</>
	)
}

function BackLink() {
	return (
		<Link
			to="/dashboard/plants"
			className="mb-1 inline-flex items-center gap-1 text-xs font-bold text-primary"
		>
			<ArrowLeft className="h-3.5 w-3.5" /> All plants
		</Link>
	)
}

function MoistureCard({ data }: { data: MoistureChannelStatus }) {
	return (
		<div className="card-soft p-5">
			<div className="mb-3 flex items-center gap-2">
				<div className="grid h-8 w-8 place-items-center rounded-xl bg-sky/15 text-sky">
					<Droplets className="h-4 w-4" />
				</div>
				<div className="min-w-0">
					<p className="truncate text-sm font-bold text-foreground">
						{data.label || 'Moisture'}
					</p>
					<p className="text-[11px] text-muted-foreground">
						Every {data.intervalMinutes ?? 5} min • pin {data.pin ?? '—'}
					</p>
				</div>
			</div>
			<div className="grid place-items-center py-3">
				<MoistureGauge value={data.reading ?? 0} />
			</div>
		</div>
	)
}

function RelayCard({
	data,
	sending,
	watering,
	onWaterNow,
}: {
	data: RelayChannelStatus
	sending: boolean
	watering: boolean
	onWaterNow: () => void
}) {
	const state: 'ON' | 'OFF' = watering ? 'ON' : data.state

	return (
		<div className="card-soft p-5">
			<div className="grid grid-cols-[minmax(0,1fr)_auto] items-start gap-3">
				<div className="min-w-0">
					<div className="flex items-center gap-2">
						<div
							className={cn(
								'grid h-8 w-8 place-items-center rounded-xl',
								state === 'ON'
									? 'bg-primary/15 text-primary'
									: 'bg-muted text-muted-foreground'
							)}
						>
							<Power className="h-4 w-4" />
						</div>
						<div className="min-w-0">
							<p className="truncate text-sm font-bold text-foreground">
								{data.label || 'Water pump'}
							</p>
							<p className="text-[11px] text-muted-foreground">
								Runs {(data.onDurationMs ?? 5000) / 1000}s • pin {data.pin ?? '—'}
							</p>
						</div>
					</div>
				</div>
				<span
					className={cn(
						'shrink-0 rounded-full px-2.5 py-1 text-xs font-black',
						state === 'ON'
							? 'bg-primary text-primary-foreground'
							: 'bg-muted text-muted-foreground'
					)}
				>
					{state}
				</span>
			</div>

			<button
				type="button"
				onClick={onWaterNow}
				disabled={sending || state === 'ON'}
				className={cn(
					'mt-4 flex h-14 w-full items-center justify-center gap-2 rounded-2xl text-base font-black shadow-soft transition-all active:scale-[0.98]',
					state === 'ON'
						? 'bg-primary/15 text-primary'
						: 'bg-primary text-primary-foreground hover:bg-primary-dark',
					sending && 'opacity-70'
				)}
			>
				{sending ? (
					<>
						<Sprout className="h-5 w-5 animate-pulse" /> Sending…
					</>
				) : state === 'ON' ? (
					<>
						<Droplets className="h-5 w-5 animate-pulse" /> Watering now
					</>
				) : (
					<>
						<Droplets className="h-5 w-5" /> Water now
					</>
				)}
			</button>
			<p className="mt-2 text-center text-[11px] text-muted-foreground">
				Manual watering triggers the pump for the configured duration.
			</p>
		</div>
	)
}

function HistorySection({
	range,
	onRangeChange,
	loading,
	error,
	moistureHistory,
	relayHistory,
}: {
	range: Range
	onRangeChange: (range: Range) => void
	loading: boolean
	error: boolean
	moistureHistory: MoistureHistoryPoint[]
	relayHistory: RelayHistoryPoint[]
}) {
	const chartData = useMemo(
		() => moistureHistory.map(p => ({ t: new Date(p.timestamp).getTime(), v: p.reading })),
		[moistureHistory]
	)

	const events = useMemo(
		() =>
			relayHistory
				.filter(p => p.state === 'ON')
				.map(p => ({ t: new Date(p.timestamp).getTime() })),
		[relayHistory]
	)

	const fmt = (t: number) => (range === '24h' ? format(t, 'HH:mm') : format(t, 'MMM d'))

	return (
		<section className="mt-6">
			<div className="grid grid-cols-[minmax(0,1fr)_auto] items-center gap-3">
				<h2 className="min-w-0 truncate text-lg font-extrabold text-foreground">History</h2>
				<div className="flex shrink-0 rounded-full bg-muted p-1">
					{RANGES.map(r => (
						<button
							key={r}
							type="button"
							onClick={() => onRangeChange(r)}
							className={cn(
								'rounded-full px-3 py-1 text-xs font-bold transition-colors',
								range === r
									? 'bg-card text-foreground shadow-soft'
									: 'text-muted-foreground'
							)}
						>
							{r}
						</button>
					))}
				</div>
			</div>

			<div className="card-soft mt-3 p-3">
				{loading ? (
					<div className="grid h-56 place-items-center">
						<Sprout className="h-5 w-5 animate-pulse text-primary" />
					</div>
				) : error ? (
					<p className="p-6 text-center text-sm text-destructive">
						Couldn't load history. Try again.
					</p>
				) : chartData.length === 0 ? (
					<p className="p-6 text-center text-sm text-muted-foreground">
						No data in this range.
					</p>
				) : (
					<div className="h-56">
						<ResponsiveContainer width="100%" height="100%">
							<LineChart
								data={chartData}
								margin={{ top: 12, right: 12, bottom: 6, left: -12 }}
							>
								<defs>
									<linearGradient id="lineFade" x1="0" y1="0" x2="0" y2="1">
										<stop
											offset="0%"
											stopColor="var(--color-leaf)"
											stopOpacity={0.9}
										/>
										<stop
											offset="100%"
											stopColor="var(--color-leaf)"
											stopOpacity={0.4}
										/>
									</linearGradient>
								</defs>
								<CartesianGrid
									strokeDasharray="3 3"
									stroke="var(--color-border)"
									vertical={false}
								/>
								<XAxis
									dataKey="t"
									tickFormatter={fmt}
									tick={{ fill: 'var(--color-muted-foreground)', fontSize: 11 }}
									stroke="var(--color-border)"
									minTickGap={30}
								/>
								<YAxis
									domain={[0, 100]}
									tick={{ fill: 'var(--color-muted-foreground)', fontSize: 11 }}
									stroke="var(--color-border)"
									width={32}
									tickFormatter={v => `${v}`}
								/>
								<Tooltip
									contentStyle={{
										background: 'var(--color-card)',
										border: '1px solid var(--color-border)',
										borderRadius: 12,
										fontSize: 12,
									}}
									labelFormatter={t => format(t as number, 'PPpp')}
									formatter={v => [`${v}%`, 'Moisture']}
								/>
								<Line
									type="monotone"
									dataKey="v"
									stroke="url(#lineFade)"
									strokeWidth={2.5}
									dot={false}
									activeDot={{ r: 4, fill: 'var(--color-leaf)' }}
								/>
								{events.map((e, i) => (
									<ReferenceDot
										key={i}
										x={e.t}
										y={4}
										r={4}
										fill="var(--color-sky)"
										stroke="var(--color-card)"
										strokeWidth={2}
										ifOverflow="extendDomain"
									/>
								))}
							</LineChart>
						</ResponsiveContainer>
					</div>
				)}
				<div className="mt-2 flex flex-wrap items-center gap-3 border-t border-border/60 px-2 pt-2 text-[11px] text-muted-foreground">
					<span className="inline-flex items-center gap-1.5">
						<span className="h-2 w-3 rounded-sm bg-primary" /> Moisture %
					</span>
					<span className="inline-flex items-center gap-1.5">
						<span className="h-2 w-2 rounded-full bg-sky" /> Pump ran
					</span>
				</div>
			</div>
		</section>
	)
}

function DeviceInfo({ status, onRemove }: { status: Device['status']; onRemove: () => void }) {
	const [open, setOpen] = useState(false)
	const rows: [string, string][] = [
		['Firmware', status?.appliedFirmwareDate || '—'],
		[
			'Last OTA check',
			status?.lastOtaCheckTime ? format(new Date(status.lastOtaCheckTime), 'PPp') : '—',
		],
		['OTA result', status?.lastOtaResult || 'up-to-date'],
	]

	return (
		<section className="mt-6">
			<button
				type="button"
				onClick={() => setOpen(v => !v)}
				className="flex w-full items-center justify-between rounded-2xl px-2 py-3 text-left text-xs font-bold uppercase tracking-wider text-muted-foreground"
			>
				<span className="inline-flex items-center gap-2">
					<Info className="h-3.5 w-3.5" /> Device info
				</span>
				<ChevronDown className={cn('h-4 w-4 transition-transform', open && 'rotate-180')} />
			</button>
			{open && (
				<>
					<dl className="card-soft divide-y divide-border/70 px-4 py-2 text-sm">
						{rows.map(([k, v]) => (
							<div
								key={k}
								className="grid grid-cols-[auto_minmax(0,1fr)] gap-4 py-2.5"
							>
								<dt className="shrink-0 text-xs font-bold uppercase tracking-wider text-muted-foreground">
									{k}
								</dt>
								<dd className="min-w-0 truncate text-right font-semibold text-foreground">
									{v}
								</dd>
							</div>
						))}
					</dl>
					<button
						type="button"
						onClick={onRemove}
						className="mt-3 flex h-11 w-full items-center justify-center gap-2 rounded-full border border-destructive/40 bg-card text-sm font-bold text-destructive transition-colors hover:bg-destructive/5"
					>
						<Trash2 className="h-4 w-4" /> Remove device
					</button>
				</>
			)}
		</section>
	)
}

function LoadingShell() {
	return (
		<>
			<BackLink />
			<div className="mt-4 grid gap-3">
				<div className="card-soft h-72 animate-pulse bg-muted/40" />
				<div className="card-soft h-40 animate-pulse bg-muted/40" />
			</div>
		</>
	)
}

function NotFoundShell() {
	return (
		<div className="mt-8 text-center">
			<div className="mx-auto grid h-16 w-16 place-items-center rounded-3xl bg-muted text-muted-foreground">
				<Wifi className="h-7 w-7" />
			</div>
			<p className="mt-4 text-sm text-muted-foreground">
				We couldn't find that device. It may have been removed or hasn't reported in yet.
			</p>
			<Link
				to="/dashboard/plants"
				className="mt-6 inline-flex items-center gap-2 rounded-full bg-primary px-5 py-2.5 text-sm font-bold text-primary-foreground shadow-soft"
			>
				Back to plants
			</Link>
		</div>
	)
}

export default DeviceDetail
