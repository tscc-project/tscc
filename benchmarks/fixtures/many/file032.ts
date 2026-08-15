interface Item32 { id: number; name: string; }
type Key32 = string | number;
export function make32(id: number, name: string): Item32 {
    const value: Item32 = { id, name };
    return value as Item32;
}
export const item32: Item32 = make32(32, "item-32");
