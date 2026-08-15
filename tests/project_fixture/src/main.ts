import type { User } from "./types.js";
import { double, Mode } from "./lib/math.js";
const user: User = {name: "Ada"};
console.log(user.name, double<number>(4), Mode.B);
