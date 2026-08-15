interface Item44 { id: number; name: string; }
type Key44 = string | number;
export function make44(id: number, name: string): Item44 {
  const value: Item44 = {id, name};
  return value as Item44;
}
export const item44: Item44 = make44(44, "item-44");
