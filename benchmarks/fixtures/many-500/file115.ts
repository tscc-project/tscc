interface Item115 { id: number; name: string; }
type Key115 = string | number;
export function make115(id: number, name: string): Item115 {
  const value: Item115 = {id, name};
  return value as Item115;
}
export const item115: Item115 = make115(115, "item-115");
