interface Item52 { id: number; name: string; }
type Key52 = string | number;
export function make52(id: number, name: string): Item52 {
    const value: Item52 = { id, name };
    return value as Item52;
}
export const item52: Item52 = make52(52, "item-52");
