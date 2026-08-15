interface Item61 { id: number; name: string; }
type Key61 = string | number;
export function make61(id: number, name: string): Item61 {
    const value: Item61 = { id, name };
    return value as Item61;
}
export const item61: Item61 = make61(61, "item-61");
