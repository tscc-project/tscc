interface Item70 { id: number; name: string; }
type Key70 = string | number;
export function make70(id: number, name: string): Item70 {
    const value: Item70 = { id, name };
    return value as Item70;
}
export const item70: Item70 = make70(70, "item-70");
