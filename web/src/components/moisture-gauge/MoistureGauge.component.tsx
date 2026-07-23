interface MoistureGaugeProps {
	value: number
}

const RADIUS = 54
const STROKE = 10
const CIRCUMFERENCE = 2 * Math.PI * RADIUS

const classify = (value: number) => {
	if (value < 30) return { label: 'NEEDS WATER', color: 'var(--color-destructive)' }
	if (value < 50) return { label: 'GETTING DRY', color: 'var(--color-warn)' }
	return { label: 'HEALTHY', color: 'var(--color-leaf)' }
}

function MoistureGauge({ value }: MoistureGaugeProps) {
	const clamped = Math.max(0, Math.min(100, value))
	const offset = CIRCUMFERENCE - (clamped / 100) * CIRCUMFERENCE
	const { label, color } = classify(clamped)

	return (
		<div className="relative grid h-40 w-40 place-items-center">
			<svg viewBox="0 0 120 120" className="h-40 w-40 -rotate-90">
				<circle
					cx="60"
					cy="60"
					r={RADIUS}
					fill="none"
					stroke="var(--color-leaf-soft)"
					strokeWidth={STROKE}
				/>
				<circle
					cx="60"
					cy="60"
					r={RADIUS}
					fill="none"
					stroke={color}
					strokeWidth={STROKE}
					strokeDasharray={CIRCUMFERENCE}
					strokeDashoffset={offset}
					strokeLinecap="round"
				/>
			</svg>
			<div className="absolute flex flex-col items-center">
				<span className="text-4xl font-black text-foreground">
					{Math.round(clamped)}
					<span className="text-xl">%</span>
				</span>
				<span
					className="mt-0.5 text-[11px] font-bold uppercase tracking-wider"
					style={{ color }}
				>
					{label}
				</span>
			</div>
		</div>
	)
}

export default MoistureGauge
