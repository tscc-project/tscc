interface Item48 { id: number; name: string; }
type Key48 = string | number;
export function make48(id: number, name: string): Item48 {
    const value: Item48 = { id, name };
    return value as Item48;
}
export const item48: Item48 = make48(48, "item-48");
