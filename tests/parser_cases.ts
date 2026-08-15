
interface User<T> { name: string; value: T }
type Maybe<T> = T | null;

function greet(user: User<number>, suffix: string = "!"): string {
    const obj = {name: "Ada", score: 3};
    const typed: number = obj.score;
    return user.name + suffix + typed;
}

class Box implements User<number> {
    public readonly name: string = "Ada";
    private value: number = 3;
    method(x: number, y: string): string { return y + x; }
}

const literal = {name: "Ada", nested: {x: 1}};
const typedObject: {name: string; x: number} = {name: "Grace", x: 2};
console.log(greet({name: "A", value: 1}, "?"), literal.name, typedObject.x, new Box().method(2,"x"));
