import {
	createUserWithEmailAndPassword,
	signInWithEmailAndPassword,
	signInWithPopup,
	GoogleAuthProvider,
	sendPasswordResetEmail,
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

const googleProvider = new GoogleAuthProvider()

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

	loginWithGoogle: async (): Promise<User> => {
		const credential: UserCredential = await signInWithPopup(auth, googleProvider)
		return credential.user
	},

	resetPassword: (email: string): Promise<void> => sendPasswordResetEmail(auth, email),

	logout: (): Promise<void> => signOut(auth),

	onAuthStateChanged: (callback: (user: User | null) => void) =>
		onAuthStateChanged(auth, callback),

	getCurrentUser: (): User | null => auth.currentUser,
}
