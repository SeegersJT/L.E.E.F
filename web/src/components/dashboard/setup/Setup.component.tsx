import { Wifi, Router, Globe, ListTree, KeyRound } from 'lucide-react'

const steps = [
	{
		icon: Router,
		title: 'Power on your L.E.E.F.',
		body: 'Plug it in. If it has no saved WiFi yet, it will create its own tiny network. The little screen will alternate between the network name to join and an address to visit.',
	},
	{
		icon: Wifi,
		title: "Join the device's WiFi",
		body: "On your phone, open WiFi settings and connect to the network shown on the device. It won't have internet - that's expected, don't worry.",
	},
	{
		icon: Globe,
		title: 'Open 10.0.0.1 in your browser',
		body: "Type 10.0.0.1 into your browser's address bar. On many phones a setup page pops up automatically.",
	},
	{
		icon: ListTree,
		title: 'Pick your home WiFi',
		body: 'Choose your home network from the list, type its password, and tap Connect. The device restarts and joins your network.',
	},
	{
		icon: KeyRound,
		title: 'Enter the pairing code',
		body: "Once connected, the screen shows a 6-character pairing code. Go to the Plants tab, tap the + button, enter that code, and give your plant a name - that's what actually links the device to your account.",
	},
]

function Setup() {
	return (
		<>
			<div className="card-soft mb-5 p-5">
				<p className="text-sm text-foreground">
					Getting a plant online takes two parts - connecting it to your WiFi, then
					pairing it to your account. Both together take about a minute.
				</p>
			</div>

			<ol className="space-y-3">
				{steps.map((s, i) => {
					const Icon = s.icon
					return (
						<li key={i} className="card-soft p-4">
							<div className="grid grid-cols-[auto_minmax(0,1fr)] items-start gap-3">
								<div className="relative grid h-11 w-11 shrink-0 place-items-center rounded-2xl bg-primary/10 text-primary">
									<Icon className="h-5 w-5" />
									<span className="absolute -right-1 -top-1 grid h-5 w-5 place-items-center rounded-full bg-primary text-[10px] font-black text-primary-foreground">
										{i + 1}
									</span>
								</div>
								<div className="min-w-0">
									<h3 className="text-sm font-extrabold text-foreground">
										{s.title}
									</h3>
									<p className="mt-1 text-sm leading-relaxed text-muted-foreground">
										{s.body}
									</p>
								</div>
							</div>
						</li>
					)
				})}
			</ol>

			<div className="mt-6 space-y-3 rounded-2xl border border-warn/30 bg-warn/10 p-4 text-sm text-earth">
				<div>
					<p className="font-bold">WiFi network not showing up?</p>
					<p className="mt-1 text-earth/90">
						Unplug the L.E.E.F. for 10 seconds and plug it back in. It restarts into
						setup mode automatically whenever it can't reach a known WiFi network.
					</p>
				</div>
				<div>
					<p className="font-bold">Pairing code not working?</p>
					<p className="mt-1 text-earth/90">
						Codes expire 15 minutes after the device connects to WiFi. If yours has gone
						stale, restart the device - it'll connect straight to your saved WiFi and
						show a fresh code.
					</p>
				</div>
			</div>
		</>
	)
}

export default Setup
