interface Item99 { id: number; name: string; }
type Key99 = string | number;
export function make99(id: number, name: string): Item99 {
    const value: Item99 = { id, name };
    return value as Item99;
}
export const item99: Item99 = make99(99, "item-99");
