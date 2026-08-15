interface Item84 { id: number; name: string; }
type Key84 = string | number;
export function make84(id: number, name: string): Item84 {
    const value: Item84 = { id, name };
    return value as Item84;
}
export const item84: Item84 = make84(84, "item-84");
