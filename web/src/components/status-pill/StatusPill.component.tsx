interface StatusPillProps {
	lastSeen?: string
}

type Status = 'online' | 'stale' | 'offline'

const classify = (lastSeen?: string): Status => {
	if (!lastSeen) return 'offline'

	const seenAt = new Date(lastSeen).getTime()
	if (Number.isNaN(seenAt)) return 'offline'

	const ageMs = Date.now() - seenAt

	if (ageMs < 2 * 60 * 1000) return 'online'
	if (ageMs < 15 * 60 * 1000) return 'stale'
	return 'offline'
}

const STYLES: Record<Status, string> = {
	online: 'bg-success/15 text-success',
	stale: 'bg-warn/15 text-earth',
	offline: 'bg-muted text-muted-foreground',
}

const DOT_STYLES: Record<Status, string> = {
	online: 'bg-success',
	stale: 'bg-warn',
	offline: 'bg-muted-foreground',
}

const LABELS: Record<Status, string> = {
	online: 'Online',
	stale: 'Stale',
	offline: 'Offline',
}

function StatusPill({ lastSeen }: StatusPillProps) {
	const status = classify(lastSeen)

	return (
		<span
			className={`inline-flex items-center gap-1.5 rounded-full px-2.5 py-1 text-xs font-bold ${STYLES[status]}`}
		>
			<span className={`h-1.5 w-1.5 rounded-full ${DOT_STYLES[status]}`} />
			{LABELS[status]}
		</span>
	)
}

export default StatusPill
