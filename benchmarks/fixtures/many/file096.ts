interface Item96 { id: number; name: string; }
type Key96 = string | number;
export function make96(id: number, name: string): Item96 {
    const value: Item96 = { id, name };
    return value as Item96;
}
export const item96: Item96 = make96(96, "item-96");
