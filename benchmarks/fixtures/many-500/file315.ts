interface Item315 { id: number; name: string; }
type Key315 = string | number;
export function make315(id: number, name: string): Item315 {
  const value: Item315 = {id, name};
  return value as Item315;
}
export const item315: Item315 = make315(315, "item-315");
