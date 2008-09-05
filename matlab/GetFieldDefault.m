function [val, struct] = GetFieldDefault(struct, field, default)
% GetFieldDefault Retreives the value of a field from a structure, or
% returns the default value if the field does not exist on the structure.
% Note that one should use defaults that make sense so that operations work
% on either the expected field values or the default value.
%
% Input:
%   struct -    the structure
%   field -     the name of the field to retrieve
%   default -   the default value to return
%
% Output:
%   val -       the value of the field or the default value
%   struct -    the structure with the requested field assigned to the
%   default value if it did not exist before

if (isfield(struct, field))
    val = struct.(field);
else
    struct.(field) = default;
    val = struct.(field);
end