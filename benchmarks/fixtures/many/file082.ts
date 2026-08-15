interface Item82 { id: number; name: string; }
type Key82 = string | number;
export function make82(id: number, name: string): Item82 {
    const value: Item82 = { id, name };
    return value as Item82;
}
export const item82: Item82 = make82(82, "item-82");
