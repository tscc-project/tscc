interface User { id: number; name: string; active: boolean; }
type ID = string | number;
function greet(user: User, prefix: string = "Hello"): string {
    const status: string = user.active ? "active" : "inactive";
    return `${prefix}, ${user.name} (${status})`;
}
const users: User[] = [
    {id: 1, name: "Ada", active: true},
    {id: 2, name: "Grace", active: false},
];
for (const user of users) console.log(greet(user));
