interface Item97 { id: number; name: string; }
type Key97 = string | number;
export function make97(id: number, name: string): Item97 {
    const value: Item97 = { id, name };
    return value as Item97;
}
export const item97: Item97 = make97(97, "item-97");
