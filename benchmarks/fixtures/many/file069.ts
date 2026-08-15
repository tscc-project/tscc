interface Item69 { id: number; name: string; }
type Key69 = string | number;
export function make69(id: number, name: string): Item69 {
    const value: Item69 = { id, name };
    return value as Item69;
}
export const item69: Item69 = make69(69, "item-69");
