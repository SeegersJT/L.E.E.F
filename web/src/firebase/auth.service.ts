import {
	createUserWithEmailAndPassword,
	signInWithEmailAndPassword,
	signOut,
	onAuthStateChanged,
	updateProfile,
	type User,
	type UserCredential,
} from 'firebase/auth'
import { auth } from './config'

export interface AuthCredentials {
	email: string
	password: string
}

export interface RegisterCredentials extends AuthCredentials {
	displayName: string
}

export const authService = {
	register: async ({ email, password, displayName }: RegisterCredentials): Promise<User> => {
		const credential: UserCredential = await createUserWithEmailAndPassword(
			auth,
			email,
			password
		)
		await updateProfile(credential.user, { displayName })
		return credential.user
	},

	login: async ({ email, password }: AuthCredentials): Promise<User> => {
		const credential: UserCredential = await signInWithEmailAndPassword(auth, email, password)
		return credential.user
	},

	logout: (): Promise<void> => signOut(auth),

	onAuthStateChanged: (callback: (user: User | null) => void) =>
		onAuthStateChanged(auth, callback),

	getCurrentUser: (): User | null => auth.currentUser,
}
