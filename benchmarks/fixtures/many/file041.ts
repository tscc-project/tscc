interface Item41 { id: number; name: string; }
type Key41 = string | number;
export function make41(id: number, name: string): Item41 {
    const value: Item41 = { id, name };
    return value as Item41;
}
export const item41: Item41 = make41(41, "item-41");
