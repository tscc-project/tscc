interface Item73 { id: number; name: string; }
type Key73 = string | number;
export function make73(id: number, name: string): Item73 {
    const value: Item73 = { id, name };
    return value as Item73;
}
export const item73: Item73 = make73(73, "item-73");
